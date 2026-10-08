#include "global.h"
u8 *Script_GetValue(void);
u32 sub_08030B78(u8 *);
u32 sub_08030BE8(void) { return sub_08030B78(Script_GetValue()); }
