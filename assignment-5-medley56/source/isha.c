/*
 * isha.c
 *
 * A completely insecure and bad hashing algorithm, based loosely on
 * SHA-1 (which is itself no longer considered a good hashing
 * algorithm)
 *
 * Based on code for sha1 processing from Paul E. Jones, available at
 * https://www.packetizer.com/security/sha1/
 */

#include "stdbool.h"
#include "isha.h"
#include "static_profiler.h"
#include "fsl_debug_console.h"

// Do not modify these declarations
uint32_t ISHAProcessMessageBlockEnd, ISHAPadMessageEnd, ISHAResetEnd,
		ISHAResultEnd, ISHAInputEnd;
#define record_pc(x)  asm("mov %0, pc" : "=r"(x))
//----------------------------------------------------------------------

/*
 * circular shift macro
 */
#define ISHACircularShift(bits,word) \
  ((((word) << (bits)) & 0xFFFFFFFF) | ((word) >> (32-(bits))))

/*  
 * Processes the next 512 bits of the message stored in the MBlock
 * array.
 *
 * Parameters:
 *   ctx         The ISHAContext (in/out)
 */
static void ISHAProcessMessageBlock(ISHAContext *ctx) {
	uint32_t temp;
	uint32_t W;
	uint32_t A, B, C, D, E;

	A = ctx->MD[0];
	B = ctx->MD[1];
	C = ctx->MD[2];
	D = ctx->MD[3];
	E = ctx->MD[4];

	#pragma GCC unroll 16
	for (int t = 0; t < 16; t++) {
		W = __builtin_bswap32(*(uint32_t *) &(ctx->MBlock[t * 4]));

		temp = ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + W;
		temp &= 0xFFFFFFFF;
		E = ISHACircularShift(25, D);
		D = ISHACircularShift(15, C);
		C = ISHACircularShift(30, B);
		B = ISHACircularShift(10, A);
		A = ISHACircularShift(5, temp);
	}

	ctx->MD[0] = (ctx->MD[0] + A);
	ctx->MD[1] = (ctx->MD[1] + B);
	ctx->MD[2] = (ctx->MD[2] + C);
	ctx->MD[3] = (ctx->MD[3] + D);
	ctx->MD[4] = (ctx->MD[4] + E);

	ctx->MB_Idx = 0;

	// Count function call if static profiling is enabled
	if (static_profiling_on) {
		ISHAProcessMessageBlockCount++;
	}

	// Do not modify this line
	record_pc(ISHAProcessMessageBlockEnd);
}

/*  
 * The message must be padded to an even 512 bits.  The first padding
 * bit must be a '1'.  The last 64 bits represent the length of the
 * original message.  All bits in between should be 0. This function
 * will pad the message according to those rules by filling the MBlock
 * array accordingly. It will also call ISHAProcessMessageBlock()
 * appropriately. When it returns, it can be assumed that the message
 * digest has been computed.
 *
 * Parameters:
 *   ctx         The ISHAContext (in/out)
 */
static void ISHAPadMessage(ISHAContext *ctx) {
	/*
	 *  Check to see if the current message block is too small to hold
	 *  the initial padding bits and length.  If so, we will pad the
	 *  block, process it, and then continue padding into a second
	 *  block.
	 */
	if (ctx->MB_Idx > 55) {
		ctx->MBlock[ctx->MB_Idx++] = 0x80;
		while (ctx->MB_Idx < 64) {
			ctx->MBlock[ctx->MB_Idx++] = 0;
		}

		ISHAProcessMessageBlock(ctx);

		while (ctx->MB_Idx < 56) {
			ctx->MBlock[ctx->MB_Idx++] = 0;
		}
	} else {
		ctx->MBlock[ctx->MB_Idx++] = 0x80;
		while (ctx->MB_Idx < 56) {
			ctx->MBlock[ctx->MB_Idx++] = 0;
		}
	}

	/*
	 *  Store the message length as the last 8 octets
	 */
	ctx->MBlock[56] = (ctx->Length_High >> 24) & 0xFF;
	ctx->MBlock[57] = (ctx->Length_High >> 16) & 0xFF;
	ctx->MBlock[58] = (ctx->Length_High >> 8) & 0xFF;
	ctx->MBlock[59] = (ctx->Length_High) & 0xFF;
	ctx->MBlock[60] = (ctx->Length_Low >> 24) & 0xFF;
	ctx->MBlock[61] = (ctx->Length_Low >> 16) & 0xFF;
	ctx->MBlock[62] = (ctx->Length_Low >> 8) & 0xFF;
	ctx->MBlock[63] = (ctx->Length_Low) & 0xFF;

	ISHAProcessMessageBlock(ctx);

	// Count function call if static profiling is enabled
	if (static_profiling_on) {
		ISHAPadMessageCount++;
	}
	// Do not modify this line
	asm("mov %0, pc" : "=r"(ISHAPadMessageEnd));
}

void ISHAReset(ISHAContext *ctx) {
	ctx->Length_Low = 0;
	ctx->Length_High = 0;
	ctx->MB_Idx = 0;

	ctx->MD[0] = 0x67452301;
	ctx->MD[1] = 0xEFCDAB89;
	ctx->MD[2] = 0x98BADCFE;
	ctx->MD[3] = 0x10325476;
	ctx->MD[4] = 0xC3D2E1F0;

	ctx->Computed = 0;
	ctx->Corrupted = 0;

	// Count function call if static profiling is enabled
	if (static_profiling_on) {
		ISHAResetCount++;
	}

	// Do not modify this line
	record_pc(ISHAResetEnd);
}

void ISHAResult(ISHAContext *ctx, uint8_t *digest_out) {
	if (ctx->Corrupted) {
		return;
	}

	if (!ctx->Computed) {
		ISHAPadMessage(ctx);
		ctx->Computed = 1;
	}

	for (int i = 0; i < ISHA_DIGESTLEN / 4; i++) {
		*((uint32_t *) &(digest_out[i*4])) = __builtin_bswap32(ctx->MD[i]);
	}
	// Count function call if static profiling is enabled
	if (static_profiling_on) {
		ISHAResultCount++;
	}

	// Do not modify this line
	record_pc(ISHAResultEnd);
}

void ISHAInput(ISHAContext *ctx, const uint8_t *message_array, size_t length) {
	if (!length) {
		return;
	}

	if (ctx->Computed || ctx->Corrupted) {
		ctx->Corrupted = 1;
		return;
	}

	if (length == ISHA_DIGESTLEN) {
		// This is the case almost always, t is 20 bytes, length is 20
		memcpy(ctx->MBlock + ctx->MB_Idx, message_array, ISHA_DIGESTLEN);
		ctx->MB_Idx += ISHA_DIGESTLEN;
		ctx->Length_Low = length * 8;  // 20 * 8
	} else {
		// Exceptional case where length != ISHA_DIGESTLEN (i.e. on the first iteration only)
		while (length-- && !ctx->Corrupted) {
			ctx->MBlock[ctx->MB_Idx++] = (*message_array & 0xFF);

			ctx->Length_Low += 8;
			if (ctx->Length_Low == 0) {
				ctx->Length_High++;
				if (ctx->Length_High == 0) {
					/* Message is too long */
					ctx->Corrupted = 1;
				}
			}

			if (ctx->MB_Idx == 64) {
				ISHAProcessMessageBlock(ctx);
			}

			message_array++;
		}
	}

	// Count function call if static profiling is enabled
	if (static_profiling_on) {
		ISHAInputCount++;
	}

	record_pc(ISHAInputEnd);
}

// Do not modify anything below this line

static bool cmp_bin(const uint8_t *b1, const uint8_t *b2, size_t len) {
	for (size_t i = 0; i < len; i++)
		if (b1[i] != b2[i])
			return false;
	return true;
}

void GetFunctionAddress(const char *func_name, uint32_t *start, uint32_t *end) {
	if (cmp_bin((const uint8_t*) func_name,
			(const uint8_t*) "ISHAProcessMessageBlock", 23)) {
		*start = (uint32_t) ISHAProcessMessageBlock;
		*end = ISHAProcessMessageBlockEnd;
	} else if (cmp_bin((const uint8_t*) func_name,
			(const uint8_t*) "ISHAPadMessage", 14)) {
		*start = (uint32_t) ISHAPadMessage;
		*end = ISHAPadMessageEnd;
	} else if (cmp_bin((const uint8_t*) func_name, (const uint8_t*) "ISHAReset",
			9)) {
		*start = (uint32_t) ISHAReset;
		*end = ISHAResetEnd;
	} else if (cmp_bin((const uint8_t*) func_name,
			(const uint8_t*) "ISHAResult", 10)) {
		*start = (uint32_t) ISHAResult;
		*end = ISHAResultEnd;
	} else if (cmp_bin((const uint8_t*) func_name, (const uint8_t*) "ISHAInput",
			9)) {
		*start = (uint32_t) ISHAInput;
		*end = ISHAInputEnd;
	}

	// Modified with Lalit's permission
	*end += 30;  // Original value was 500

}

