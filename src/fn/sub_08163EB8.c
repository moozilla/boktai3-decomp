#include "global.h"

struct P {
    u8 f00[0xA4E];
    u16 h4e;
    u8 fa50[4];
    u32 w54;
    u8 c58;
};

void PauseMenu_LoadSpriteKeyframe(struct P *, s32, s32);

void Hud_SetInteractionCallback(struct P *p, u32 w, u32 c)
{
    p->w54 = w;
    p->h4e = 0;
    p->c58 = c;
    if ((c << 24) != 0) {
        PauseMenu_LoadSpriteKeyframe(p, 1, 0x43);
        PauseMenu_LoadSpriteKeyframe(p, 2, 0x44);
    } else {
        PauseMenu_LoadSpriteKeyframe(p, 1, 0x41);
        PauseMenu_LoadSpriteKeyframe(p, 2, 0x42);
    }
}
