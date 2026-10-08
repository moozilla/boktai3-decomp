#include "global.h"
struct VM { u8 p0[8]; u32 *f8; };
extern struct VM gUnk_02000610;
void Script_PopCtrlNextKeyword(void) { gUnk_02000610.f8--; }
