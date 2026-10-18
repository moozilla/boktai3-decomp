#include "global.h"
#define TransitionPanel_UpdateOpening sub_08162F5C
struct TransitionPanelSprite {
    u32 flags;
    u8 unknown4[0x4C];
};
struct TransitionPanel {
    u8 task[0x18];
    struct TransitionPanelSprite sprites[8];
    u8 unknown298[0xA4];
    s16 extentA, extentB;
    u8 unknown340, state;
    u8 unknown342[4];
    u16 timer;
};
void Sound_PlaySong(u32);
void sub_08162F5C(struct TransitionPanel *p)
{
    u32 timer = p->timer + 1;
    p->timer = timer;
    if ((u16)timer == 30) {
        Sound_PlaySong(0x2F9);
    } else if ((u16)timer > 149) {
        if ((u16)timer == 150)
            Sound_PlaySong(0x12D);
        p->extentA += 5;
        p->extentB -= 5;
        if (p->extentA > 127) {
            struct TransitionPanelSprite *sprite;
            s32 i;
            p->extentA = 1;
            p->extentB = 1;
            {
                u32 mask = 1;
                sprite = p->sprites;
                for (i = 7; i >= 0; i--) {
                    sprite->flags |= mask;
                    sprite++;
                }
            }
            {
                u32 zero = 0;
                p->state = 7;
                p->timer = zero;
            }
        }
    }
}
