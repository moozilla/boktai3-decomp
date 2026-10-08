#include "global.h"
u8 *Script_GetPc(void);
u32 sub_0821AA00(u8 *);
u32 Script_GetValue(void) { return sub_0821AA00(Script_GetPc()); }
