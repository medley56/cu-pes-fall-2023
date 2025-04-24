/*
 * state_machine.c
 *
 *  Created on: Oct 17, 2023
 *      Author: Gavin Medley
 */

#include <stdbool.h>
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"
#include "board.h"
#include "pin_mux.h"
#include "log.h"
#include "timing.h"
#include "led.h"
#include "tsi.h"
#include "switch.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#ifdef DEBUG
	#define T_WAIT_TICKS 3U * SEC_FRAC  // 3s
	#define A_WAIT_TICKS 2U * SEC_FRAC  // 2s
	#define B_WAIT_TICKS 2U * SEC_FRAC  // etc.
	#define S_WAIT_TICKS 1U * SEC_FRAC
#else
	#define T_WAIT_TICKS 5U * SEC_FRAC
	#define A_WAIT_TICKS 4U * SEC_FRAC
	#define B_WAIT_TICKS 4U * SEC_FRAC
	#define S_WAIT_TICKS 3U * SEC_FRAC
#endif

#define EMCY_BLINK_TICKS (SEC_FRAC / 2U)  // Half of one second
#define EMERGENCY_RED_IDX 17U  // Reliable way to reference RED emergency state in table
#define EMERGENCY_PREV_IDX 18U  // Reliable way to reference PREVIOUS emergency state in table
#define TRANS_WAIT_TICKS (SEC_FRAC / 2U)  // Half of one second

#define INITIAL_STATE_IDX 0  // Start at Terminal
#define INITIAL_DIRECTION REVERSE  // Start in reverse because Terminal turns the train around

/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/* State type forward declaration */
typedef struct state state_t;

/* Function pointer type definition for a state function event handler */
typedef state_t (*event_handler_fxn_t) (state_t state);

/* Data type for current state */
typedef struct state {
	uint8_t index;
	char name[16];
	uint32_t color;
	uint32_t wait_ticks;
	uint8_t next_fwd;
	uint8_t next_rev;
	bool turn;
	event_handler_fxn_t timeout_handler;
	event_handler_fxn_t touch_handler;
	event_handler_fxn_t switch_handler;
} state_t;

/*
 * Emergency handler (responds to touch and switch events)
 *
 * Parameters
 *   state  The current state
 *
 * Returns
 *   state_t  The next state
 */
state_t toggle_emergency(state_t state);

/*
 * Emergency blink transition handler (timeout handler when in emergency)
 *
 * Parameters
 *   state  The current state
 *
 * Returns
 *   state_t  The next state
 */
state_t blink_emergency(state_t state);

/*
 * Nominal next state transition handler (timeout handler when not in emergency)
 *
 * Parameters
 *   state  The current state
 *
 * Returns
 *   state_t  The next state
 */
state_t next_state(state_t state);


/*******************************************************************************
 * Code
 ******************************************************************************/
const state_t states[] = {
  /* idx name             color     timeout_ticks     F   R   turn   timeout_f   touch_f           switch_f       */
	{0,  "T",             0xFFFFFF, T_WAIT_TICKS,     1,  -1, true,  next_state, toggle_emergency, toggle_emergency},  // Terminal
	{1,  "T(Departing)",  0xFFFFFF, TRANS_WAIT_TICKS, 2,  -1,  false, next_state, toggle_emergency, toggle_emergency},  // Departing Terminal
	{2,  "T(75%):A(25%)", 0xBFBFFF, TRANS_WAIT_TICKS, 3,  0,  false, next_state, toggle_emergency, toggle_emergency},  // TA1
	{3,  "T(50%):A(50%)", 0x7F7FFF, TRANS_WAIT_TICKS, 4,  2,  false, next_state, toggle_emergency, toggle_emergency},  // TA2
	{4,  "T(25%):A(75%)", 0x4040FF, TRANS_WAIT_TICKS, 5,  3,  false, next_state, toggle_emergency, toggle_emergency},  // TA3
	{5,  "A",             0x0000FF, A_WAIT_TICKS,     6,  6,  false, next_state, toggle_emergency, toggle_emergency},  // Concourse A
	{6,  "A(Departing)",  0x0000FF, TRANS_WAIT_TICKS, 7,  4, false, next_state, toggle_emergency, toggle_emergency},  // Departing A
	{7,  "A(75%):B(25%)", 0x0040BF, TRANS_WAIT_TICKS, 8,  5,  false, next_state, toggle_emergency, toggle_emergency},  // AB1                                                                                                                                                              false, next_state, toggle_emergency},
	{8,  "A(50%):B(50%)", 0x007F7F, TRANS_WAIT_TICKS, 9,  7,  false, next_state, toggle_emergency, toggle_emergency},  // AB2
	{9,  "A(25%):B(75%)", 0x00BF40, TRANS_WAIT_TICKS, 10, 8,  false, next_state, toggle_emergency, toggle_emergency},  // AB3
	{10, "B",             0x00FF00, B_WAIT_TICKS,     11, 11,  false, next_state, toggle_emergency, toggle_emergency},  // Concourse B
	{11, "B(Departing)",  0x00FF00, TRANS_WAIT_TICKS, 12, 9, false, next_state, toggle_emergency, toggle_emergency},  // Departing B
	{12, "B(75%):S(25%)", 0x40FF00, TRANS_WAIT_TICKS, 13, 10, false, next_state, toggle_emergency, toggle_emergency},  //  BS1
	{13, "B(50%):S(50%)", 0x7FFF00, TRANS_WAIT_TICKS, 14, 12, false, next_state, toggle_emergency, toggle_emergency},  // BS2
	{14, "B(25%):S(75%)", 0xBFFF00, TRANS_WAIT_TICKS, 15, 13, false, next_state, toggle_emergency, toggle_emergency},  // BS3
	{15, "S",             0xFFFF00, S_WAIT_TICKS,     16, 16, true,  next_state, toggle_emergency, toggle_emergency},  // Shed
	{16, "S(Departing)",  0xFFFF00, TRANS_WAIT_TICKS, -1, 14, false, next_state, toggle_emergency, toggle_emergency},  // Departing Shed
	{17, "!RED!",         0xFF0000, EMCY_BLINK_TICKS, 18, 18, false, blink_emergency, toggle_emergency, toggle_emergency},  // Emergency blink red
	{18, "!PREV!",        0x000000, EMCY_BLINK_TICKS, 17, 17, false, blink_emergency, toggle_emergency, toggle_emergency}   // Emergency blink previous
};

const char direction_names[2][4] = {"FWD", "REV"};  // Direction names, 0=fwd, 1=rev
const enum {
	FORWARD,
	REVERSE
} directions;
const state_t emergency_red_state = states[EMERGENCY_RED_IDX];  // Shortcut to access E state
state_t pre_emergency_state;  // State to return to after emergency
state_t state;  // Global state

/* Handler for touch detection. Either enter or exit an emergency state. */
state_t toggle_emergency(state_t state)
{
	static uint32_t wait_ticks;
	if (state.index == EMERGENCY_RED_IDX || state.index == EMERGENCY_PREV_IDX)  // Crrently in emergency => exit emergency
	{
		/* We get here if we experienced an emergency but it is now over
		 * Return to normal operation using the previously stored wait_ticks value
		 * rather than the default value provided by the state. */
		LOG("[%u] END EMERGENCY: %s --> %s", now(), state.name, pre_emergency_state.name);
		pre_emergency_state.wait_ticks = wait_ticks;
		return pre_emergency_state;

	} else  // Not currently in emergency => enter emergency
	{
		LOG("[%u] BEGIN EMERGENCY: %s --> %s", now(), state.name, emergency_red_state.name);
		/* Save remaining wait time to wait_ticks so we can come back to it */
		wait_ticks = state.wait_ticks - get_global_timer();
		/* Keep track of where the emergency is occurring */
		pre_emergency_state = state;
		/* EMERGENCY */
		return emergency_red_state;
	}
}


/* Moves back and forth between emergency LED on and emergency LED off states */
state_t blink_emergency(state_t state)
{
	LOG("[%u] IN EMERGENCY: %s --> %s", now(), state.name, states[state.next_fwd].name);
	state = states[state.next_fwd];
	if (state.index == EMERGENCY_PREV_IDX)
	{
		state.color = pre_emergency_state.color;
	}
	return state;
}


/* Sets the next state based on the current state, assuming normal operation (no emergency). */
state_t next_state(state_t state)
{
	/* We initialize this to REVERSE because our first state is the terminal,
	 * which changes the direction as the train leaves it */
	static bool direction = INITIAL_DIRECTION;  // Forward = 0 = towards Shed. Reverse = 1 = towards Terminal

	/* Check if this state is a turnaround point and switch direction if so */
	if (state.turn)
	{
		LOG("[%u] SET DIRECTION: %s -> %s", now(), direction_names[direction], direction_names[!direction]);
		direction = !direction;
	}

	/* Check if we are going forward (false-0) or reverse (true-1) */
	if (!direction)
	{
		LOG("[%u] TRAIN MOVING: %s --> %s", now(), state.name, states[state.next_fwd].name);
		state = states[state.next_fwd];
	} else
	{
		LOG("[%u] TRAIN MOVING: %s --> %s", now(), state.name, states[state.next_rev].name);
		state = states[state.next_rev];
	}
	return state;
}

/* Run main state machine loop */
void state_machine_run()
{
	LOG("[%u] START MAIN LOOP", now());
	while (1)
	{
		/* Respond to events */

		/* Disable touch and switch IRQs so that our event response is not subject to races
		 * Systick continues to count. */
		NVIC_DisableIRQ(TSI_IRQn);
		NVIC_DisableIRQ(SWITCH_IRQn);

		if (tsi_touched)
		{
			LOG("[%u] TOUCH DETECTED", now());
			tsi_touched = false;  // Reset tsi_touched flag to false
			state = state.touch_handler(state);
			reset_global_timer();

		} else if (switch_changed)
		{
			LOG("[%u] SWITCH CHANGE DETECTED", now());
			switch_changed = false;  // Reset switch changed flag to false
			state = state.switch_handler(state);
			reset_global_timer();

		} else if (get_global_timer() >= state.wait_ticks)
		{
			LOG("[%u] TIMEOUT DETECTED", now());
			state = state.timeout_handler(state);
			reset_global_timer();
		}

		/* Re-enable IRQ */
		NVIC_EnableIRQ(TSI_IRQn);
		NVIC_EnableIRQ(SWITCH_IRQn);

		/* Set LED color based on current state */
		led_control_hex(state.color);
	}
}

void state_machine_init()
{
	tsi_init();  // TSI is required for systick
	systick_init();  // systick starts TSI scan on tick
	led_init();
	switch_init();

	state = states[INITIAL_STATE_IDX];
}

