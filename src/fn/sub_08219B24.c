#include "global.h"
#include "render.h"

struct PadState {
    u16 held;
    u16 pressed;
    u16 released;
    u16 pad;
};
extern u16 gUnk_03005270;
extern u32 gUnk_03005250;
extern struct PadState gUnk_03005260[2];
extern u16 gUnk_03005274[2];

#define PAD_UPDATE(i, k)                                                                           \
    {                                                                                              \
        old = gUnk_03005260[i].held;                                                               \
        gUnk_03005260[i].held = k;                                                                 \
        gUnk_03005260[i].pressed = k & ~old;                                                       \
        gUnk_03005260[i].released = old & ~k;                                                      \
    }

void sub_08219B24(void)
{
    s32 i;
    u16 keys, old;

    gUnk_03005270 = *(vu16 *)0x04000130;
    if (gUnk_03005250 == 0) {
        keys = 0x3FF ^ gUnk_03005270;
        PAD_UPDATE(0, keys);
        for (i = 1; i < 2; i++) {
            keys = 0;
            PAD_UPDATE(i, keys);
        }
    } else if (!(gEnginePauseFlags & 1)) {
        for (i = 0; i < 2; i++) {
            if (gUnk_03005274[i] == 0xFFFF)
                keys = 0;
            else
                keys = 0x3FF ^ gUnk_03005274[i];
            PAD_UPDATE(i, keys);
        }
    }
}
