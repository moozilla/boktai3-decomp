#include "global.h"
extern u16 gUnk_020005A0;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0816CD6C(void) {
    s32 i;
    gUnk_020005A0 = 0;
    if (Script_SeekToKeyword(0x66)) {
        for (i = 0; i <= 3; i++) {
            if (Script_GetValue()) gUnk_020005A0 |= 1 << i;
        }
    }
}
