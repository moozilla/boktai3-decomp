#include "global.h"
#include "ram.h"
struct P {
    u8 f0[0x342];
    u8 e;
};
void Sprite_Remove(u8 *);
void Anim_Remove(u8 *);
u32 GameOver_OnDestroy(u8 *p)
{
    u8 *q;
    s32 i;
    if (((struct P *)p)->e != 0) {
        q = p + 0x18;
        for (i = 7; i >= 0; i--) {
            Sprite_Remove(q);
            q += 0x50;
        }
        Anim_Remove(p + 0x2b8);
    }
    return gGameOver = 0;
}
