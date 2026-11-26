#ifndef GUARD_DEBUG_H
#define GUARD_DEBUG_H
/* Debug output that the release build compiled out. Where the ROM shows a test with nothing
   left in its body, the original most likely printed something there:
   `if (TakeFlag(f)) { DEBUG_PRINT(); }`. An empty if without it counts as a barrier. */
#define DEBUG_PRINT(...)
#endif
