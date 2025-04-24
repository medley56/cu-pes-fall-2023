# PES Assignment 3: TrafficLights

## LED Module
`led.h`

This module contains code for initializing and controlling the tri-color LED on the board.

## TSI Module
`tsi.h`

This module contains code for initializing and controlling the Touch Sensor Input capacitative touch sensor on the board.

NOTE: For convenience, the TSI configuration constants are all available in the tsi.h header file.

## Wait Timing Module
`wait.h`

This module contains a wait function for controlling the timing of events, including constants for 500ms and 1000ms.

## Module Level Logging

Each module contains a block similar to the following:
```
#ifdef DEBUG
    #define LOG(fmt, ...) PRINTF("[MAIN] " fmt, ##__VA_ARGS__)
#else
    #define LOG(fmt, ...) (void)0
#endif
```

Conditional on the presence of the `DEBUG` definition, (e.g. `gcc -DDEBUG ...`), this defines a `LOG()` macro that internally calls `PRINTF` with a prepended module context (`[MAIN]` in this case). In the absence of the `DEBUG` definition, this macro does nothing.

## Location of Code on Board
Grepped output from the symbol table produced from `objdump -x Release/TrafficLights.axf`. The Release axf and 
the corresponding objdump output are committed in the `objdump_output` directory.

```
Loc      Scope T Area   Size     Name
00001ea3 g     F .text	00000022 main
000007f1 g     F .text	0000002c wait_ms
```

The `main` function is located at address `00001ea3`.

The `wait_ms` (delay) function is located at address `000007f1`.

Memory sections
```
Sections:
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         000021f8  00000000  00000000  00010000  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  1 .data         00000008  1ffff080  000021f8  0001f080  2**2
                  CONTENTS, ALLOC, LOAD, DATA
  ...
  8 .stack        00000000  20002c00  20002c00  0001f088  2**2
                  CONTENTS
```

The `.text` segment of memory is 8696 (`000021f8`) bytes.

The `.data` segment of memory is 8 (`00000008`) bytes.

The initial `stack` size is 0 (`00000000`) bytes.

## Code Review Comments
1. Line 121, move the else in with line 122 of main.c, was originally confused thinking it was two separate ifs.
- Done.
2. poll_for_touch() is a very interesting approach to polling compared to how I executed it (clever), I would say it was a bit difficult to interpret at first though, I would advise adding comments in polling that the intent is keep the Leds on while polling for touch and returning time represents remaining led time that was interrupted by tsi led sequence.
- Added docstring to the traffic light sequence function to clarify its operation.
3. If compiler optimizations are turned off then wait() line 40 isn't needed.
- I chose not to change this because it doesn't hurt anything and it may prevent mistiming if optimization is enabled.
4. For led interface functions, the idea of turning them off and then on is smart and cleans up the code but identify what is happening to the led in the function name would be useful. While looking at it from the perspective of main.c to know what is happening
- Done. I think this was cleared up when I refactored the LED API. The only functions available now are very obvious and they have docstrings in led.h for reference.
5. I don't think you included start timer prints in your led_test_pattern() function.
- This is taken care of in the wait module. It automatically prints those logs for every wait call with a non-zero duration.
6. Discussed making functions static and sharing same name to indicate turning LEDs on better.
- Done. I cleaned up the API for led.h and made the internal functions static. 
7. Discussed adding the output of `objdump -x Release/TrafficLights.axf` as documentation.
- Done.

## Developer Documentation
To develop this project in an Eclipse IDE (i.e. MCUXpresso):
1. Clone the project: `git clone git@github.com:ECEN5813/assignment-3-medley56.git`
2. Import the project into an Eclipse workspace by choosing File -> Import -> General -> Existing Projects Into Workspace
