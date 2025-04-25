# Describe the call tree of the existing code

## ISHA Usage

The ISHA module has three main API calls: `ISHAReset`, `ISHAInput`, and `ISHAResult`, which are 
intended to be called in order:

1. ISHAReset resets the ISHAContext struct back to it's base state in preparation for starting a 
new hashing process (part of the PBKDF1 algorithm). 
2. ISHAInput adds data such as a password or a salt to the ISHAContext in the form of a byte string.
3. ISHAResult performs the hashing calculation and writes out the result to a buffer.

By iteratively feeding the result from ISHAResult back into ISHAInput, it is possible to increase 
the complexity of reversing the hash algorithm. See PDKDF1 Usage.

## PBKDF1 Usage

The PBKDF1 API conists of a single function called `pbkdf1`, which iteratively calculates the 
ISHA hash of an input password and salt. 
First it feeds in the password followed by the salt, then calculates the initial hash digest, which consists 
of 20 bytes. 
From there, `pbkdf1` iterates the ISHA hash a configurable number of times, 
each time feeding the previous hash digest value into the next iteration of the ISHA hash algorithm.

After iterating, the derived key (the final digest) is copied out to a buffer.

## `main` function

The main function:
1. Initializes the board and the ticktime module.
2. Runs the validation tests to prove that the algorithm works as expected.
3. Runs a single test that times the PBKDF1 through 4096 iterations.
4. Runs the same test as above but with static profiling (function call counting) turned on.
5. Runs the same test again but with PC profiling (statistical function sampling) turned on.

# Give an overview of the changes you have made to each file

All files were automatically reformatted for better readability and indentation consistency.

## `isha.c`

1. The `GetFunctionAddress` function was modified to only add 25 bytes to the end address rather than 
   the original 500 bytes.
2. `ISHAProcessMessageBlock`, `ISHAPadMessage`, `ISHAReset`, `ISHAInput`, `ISHAResult` 
   were all modified to conditionally increment a global counter variable if `static_profiling_on` 
   global is set.
3. #pragma unroll the loops in ISHAProcessMessageBlock (*~19ms speedup*).
4. Change out the manual bit swapping in ISHAProcessMessageBlock to use __builtin_bswap32 (1ms speedup).
5. Add an if statement to ISHAResult that optimizes for the common case when the input data has 
   length ISHA_DIGESTLEN bytes. (*~47ms speedup*).
6. Change the endian swap in ISHAResult to use __builtin_bswap32 by pretending like digest_out is a pointer to a 32b 
   value instead of an 8b value and modifying the loop accordingly. (~6ms speedup).

## `static_profiler.c`

1. The API was implemented to set/unset the `static_profiling_on` global and to print static 
call counter values for each of the ISHA module functions.

## `pc_profiler.c`

1. The API was implemented to set/unset the `pc_profiling_on` global variable.
2. `pc_profile_check` was implemented to check PC values against previously stored address 
ranges for each function of interest.
3. `print_pc_profiler_summary` was implemented to to print the results.

## `systick.c`

1. The systick IRQ handler was modified to extract the previous program counter value from the stack 
and call the PC profiler to increment the count for the function that was interrupted.
2. The systick frequency calculation (LOAD value) was modified to use a single `TICKS_PER_SECOND` definition and 
the units of the global counters were changed to count in generic `ticks` instead of hardcoding a specific 
unit for a system tick. 
3. The timers still return time in ms.

## Changes to configuration and compiler options

1. Compile with `-O3` option in Release mode
2. Use the `-fno-builtin` flag to compiler to prevent GCC from using the builtin GCC versions of standard functions. (*~20ms speedup*).


# Give the call count before optimization of each function in isha.c using static counters

These results were obtained before any optimization (no changes to the ISHA 
module). The results for call counting profiling were unchanged when 
compiling with `-O0` vs `-O3`, which was expected.

```
Running call count test with static profiling....
time_pbkdf1: 4096 iterations complete
Static profiling results:
ISHAProcessMessageBlockCount = 4096
ISHAPadMessageCount = 4096
ISHAResetCount = 4096
ISHAInputCount = 4097
ISHAResultCount = 4096
End of static profiling results
Done with call count test with static profiling....
```

Note: The "extra" call to `ISHAInput` is a result of separately passing both the 
password text and the salt text to the hashing algorithm before starting the 
hashing iteration.

# Give the call count before optimization of each function in isha.c using pc profiling technique

## Results using `-O0` in DEBUG mode

```
Running call count test with PC profiling....
Storing address ranges for each function of interest...
ISHAProcessMessageBlock start=1405, end=1797
ISHAPadMessage start=1797, end=2105
ISHAReset start=2105, end=2219
ISHAInput start=2465, end=2675
ISHAResult start=2237, end=2465
time_pbkdf1: 4096 iterations complete
PC profiling results:
ISHAProcessMessageBlockCountPC = 12289
ISHAPadMessageCountPC = 7165
ISHAResetCountPC = 0
ISHAInputCountPC = 8192
ISHAResultCountPC = 3073
UnknownCountPC = 166
End of PC profiling results
Done with call count test with PC profiling....
```

The total number of PC samples was 30885 (number of interrupts between turning PC profiling on/off).

This indicates that the time spent in each sampled function is:

- ISHAProcessMessageBlock: *39.8%*
- ISHAPadMessage: 23.2%
- ISHAReset: 0%
- ISHAInput: 26.5%
- ISHAResult: 9.9%
- Other: 0.5%

## Results using `-O3` in DEBUG mode

```
Running call count test with PC profiling....
Storing address ranges for each function of interest...
ISHAProcessMessageBlock start=2437, end=3903
ISHAPadMessage start=3905, end=4347
ISHAReset start=4617, end=4695
ISHAInput start=4861, end=4997
ISHAResult start=4705, end=4849
time_pbkdf1: 4096 iterations complete
PC profiling results:
ISHAProcessMessageBlockCountPC = 4096
ISHAPadMessageCountPC = 2
ISHAResetCountPC = 0
ISHAInputCountPC = 3276
ISHAResultCountPC = 819
UnknownCountPC = 979
End of PC profiling results
Done with call count test with PC profiling....
```

The total number of PC samples was 9172 (number of interrupts between turning PC profiling on/off).

This indicates that the time spent in each sampled function is:

- ISHAProcessMessageBlock: 44.7%
- ISHAPadMessage: 0.02%
- ISHAReset: 0%
- ISHAInput: 35.7%
- ISHAResult: 8.9%
- Other: 10.7%

NOTE: When the compiler optimizes this code, it results in more PC samples falling outside the expected ranges
for the function and the samples from ISHAPadMessage drop to almost zero. This indicates that the compiler is doing something with ISHAPadMessage.

# Give the size of the .text segment from MCUXPresso before optimization

The initial size of the `.text` segment was 16328 bytes when compiled with `-O0`.

```
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         00003fc8  00000000  00000000  00010000  2**2
```

When compiled with `-Os`, the `.text` segment was reduced to 11888 bytes.

```
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         00002e70  00000000  00000000  00010000  2**2
```

# Give the output of the test run (for function times) before optimization

Compiled with `-O0` in DEBUG mode

```
Running validity tests...
test_isha test 0: success
test_isha test 1: success
test_isha test 2: success
test_isha test 3: success
test_isha test 4: success
test_isha test 5: success
test_isha test 6: success
test_isha test 7: success
test_pbkdf1 test 0: success
test_pbkdf1 test 1: success
test_pbkdf1 test 2: success
test_pbkdf1 test 3: success
test_pbkdf1 test 4: success
test_pbkdf1 test 5: success
test_pbkdf1 test 6: success
test_pbkdf1 test 7: success
test_pbkdf1 test 8: success
test_pbkdf1 test 9: success
test_pbkdf1 test 10: success
All tests passed!
Running timing test...Report this time.
time_pbkdf1: 4096 iterations took 535 msec
Done with timing test...
Running call count test with static profiling....
time_pbkdf1: 4096 iterations complete
Done with call count test with static profiling....
Running call count test with PC profiling....
time_pbkdf1: 4096 iterations complete
Done with call count test with PC profiling....
```

# Give the output the test run(for function times) afer your optimizations

Compiled with the `-O3` flag in Release mode.

```
Running validity tests...
test_isha test 0: success
test_isha test 1: success
test_isha test 2: success
test_isha test 3: success
test_isha test 4: success
test_isha test 5: success
test_isha test 6: success
test_isha test 7: success
test_pbkdf1 test 0: success
test_pbkdf1 test 1: success
test_pbkdf1 test 2: success
test_pbkdf1 test 3: success
test_pbkdf1 test 4: success
test_pbkdf1 test 5: success
test_pbkdf1 test 6: success
test_pbkdf1 test 7: success
test_pbkdf1 test 8: success
test_pbkdf1 test 9: success
test_pbkdf1 test 10: success
All tests passed!
Running timing test...Report this time.
time_pbkdf1: 4096 iterations took 94 msec
Done with timing test...
Running call count test with static profiling....
time_pbkdf1: 4096 iterations complete
Static profiling results:
Function: ISHAProcessMessageBlock    Call count: 4096
Function: ISHAPadMessage             Call count: 4096
Function: ISHAReset                  Call count: 4096
Function: ISHAInput                  Call count: 4097
Function: ISHAResult                 Call count: 4096
End of static profiling results
Done with call count test with static profiling....
Running call count test with PC profiling....
Storing address ranges for each function of interest...
ISHAProcessMessageBlock start=2525, end=3566
ISHAPadMessage start=3565, end=4012
ISHAReset start=2437, end=2520
ISHAInput start=4373, end=4518
ISHAResult start=4277, end=4366
time_pbkdf1: 4096 iterations complete
PC profiling results (Call count indicates the number of PC samples taken in the function):
Function: ISHAProcessMessageBlock    Call count: 548
Function: ISHAPadMessage             Call count: 58
Function: ISHAReset                  Call count: 58
Function: ISHAInput                  Call count: 86
Function: ISHAResult                 Call count: 29
Function: Unknown                    Call count: 205
End of PC profiling results
Done with call count test with PC profiling....
```

# Give the size of the .text segment from MCUXPresso after optimization

Compiled with the `-O3` flag, the text segment was 13808 bytes.

```
Idx Name          Size      VMA       LMA       File off  Algn
  0 .text         000035f0  00000000  00000000  00010000  2**2
```

