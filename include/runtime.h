#ifndef GUARD_RUNTIME_H
#define GUARD_RUNTIME_H

#include "global.h"

/* RAM objects; each row of symbols/ram.csv with header include/runtime.h. */
extern u8 gIwramCode
    []; /* IWRAM instruction buffer populated from Palette_CacheObjectBankArm through AgbMain__addr at startup; extent is not inferred from the partial RFU read */

#endif
