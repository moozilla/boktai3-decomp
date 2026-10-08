#include "global.h"
u8 *Script_GetPc(void);
u32 sub_0821AA88(u8 *);
u32 Script_GetValueSafe(void) { return sub_0821AA88(Script_GetPc()); }
