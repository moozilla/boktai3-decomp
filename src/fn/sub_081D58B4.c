#include "global.h"
void MonsterBook_UpdateHighlightPulse(u8 *arg)
{
    u8 *p = arg;
    s32 v = *((s16 *)(p + 0xD4C));
    u32 out = 0x1f;
    do {
        if (v > 9) {
            out = 0x1b;
            if (v > 0x11) {
                out = 0x12;
                if (v > 0x19) {
                    out = 10;
                    if (v > 0x21) {
                        out = 0x1b;
                        if (v <= 0x29) {
                            out = 0x12;
                        }
                    }
                }
            }
        }
        (*((u16 *)(p + 0xD4C)))++;
        if ((*((s16 *)(p + 0xD4C))) > 0x31) {
            *((u16 *)(p + 0xD4C)) = 0;
        }
    } while (0);
    *((u16 *)(p + 0xD44)) = out;
}
