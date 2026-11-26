#ifndef GUARD_RANDOM_H
#define GUARD_RANDOM_H

#include "engine/shared/engine_globals.h"
#include "engine/shared/ram.h"

/* Advance the shared random cursor and read the table entry it lands on. The table address
 * is formed after the cursor is stored (pointer arithmetic, not an array subscript). */
#define RANDOM_NEXT() (*(gRandomTable + (gRandomIndex = (gRandomIndex + 1) & 0x3FF)))

#endif
