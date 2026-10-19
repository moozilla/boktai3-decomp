#include "global.h"
#define MenuSelection_UpdateCursorSprites sub_0817597C
s32 Anim_LoadKeyframe(void *, void *, u32, u32);
struct MenuCursorSprite {
    u8 unk00[8];
    u32 flags;
};
void sub_0817597C(u8 *s, s32 visible)
{
    u8 *data;
    if (!visible) {
        {
            struct MenuCursorSprite *sprite = (struct MenuCursorSprite *)(s + 0x1600);
            sprite->flags |= 1;
        }
        {
            struct MenuCursorSprite *sprite = (struct MenuCursorSprite *)(s + 0x3760);
            sprite->flags |= 1;
        }
        {
            struct MenuCursorSprite *sprite = (struct MenuCursorSprite *)(s + 0x37C0);
            sprite->flags |= 1;
        }
    } else {
        *(u32 *)(s + 0x1608) &= ~1;
        *(u16 *)(s + 0x1620) = 0;
        {
            u32 y = (s[0x48C0] << 4) + 24;
            *(u16 *)(s + 0x1622) = y;
        }
        {
            struct MenuCursorSprite *sprite = (struct MenuCursorSprite *)(s + 0x3760);
            sprite->flags &= ~1;
        }
        {
            struct MenuCursorSprite *sprite = (struct MenuCursorSprite *)(s + 0x37C0);
            sprite->flags &= ~1;
        }
        *(u16 *)(s + 0x3780) = 128;
        *(u16 *)(s + 0x37E0) = 216;
        {
            u32 y = (s[0x48C0] << 4) + 24;
            *(u16 *)(s + 0x3782) = y;
            *(u16 *)(s + 0x37E2) = y;
        }
    }
    Anim_LoadKeyframe(s + 0x26E0, data = s + 0x64, s[0x48C1] + 0x4D, 1);
    Anim_LoadKeyframe(s + 0x2740, data, s[0x48C2] + 0x4D, 1);
    {
        u32 base = 0x4E;
        Anim_LoadKeyframe(s + 0x27A0, data, (u16)(base - s[0x48C3]), 1);
    }
}
