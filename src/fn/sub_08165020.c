#include "global.h"

#ifdef NONMATCHING
// fuzzy: 96.6%
// Remaining differences: instruction selection, load placement, and literal-mask materialization.
// confidence=low: icon identity inferred from timed player status and save flags.
#define Hud_UpdateTimedStatusIcon sub_08165020
extern u8 *gUnk_02000710;
s32 sub_0822D604(s32);
void Anim_LoadKeyframe(void *, void *, u16, s32);
u16 *Hud_GetTilemapCell(s32, s32, s32);
s32 Hud_GetStyleTileIndex(s32, s32);
s32 Div(s32, s32);
void Hud_UpdateTimedStatusIcon(u8 *p)
{
    u32 *player = (u32 *)(p + 0xa40);
    u16 *cell;
    s32 value, digit;
    u8 *anim, *res;
    if (*(u16 *)((u8 *)*player + 0x538)) {
        *(u32 *)(p + 0xaa4) &= ~1;
        anim = p + 0xa9c;
        res = p + 0xa4;
        Anim_LoadKeyframe(anim, res, sub_0822D604(*(u16 *)((u8 *)*player + 0x536)), 1);
        cell = Hud_GetTilemapCell(0, 1, 6);
        value = Div(*(u16 *)((u8 *)*player + 0x538) + 59, 60);
        digit = Div(value, 10);
        value -= digit * 10;
        *cell++ = Hud_GetStyleTileIndex(digit, 0) | 0xffffd000;
        *cell = Hud_GetStyleTileIndex(value, 0) | 0xffffd000;
        p[0xafc] = 1;
    } else if (*(s16 *)(gUnk_02000710 + 0x610)) {
        *(u32 *)(p + 0xaa4) &= ~1;
        anim = p + 0xa9c;
        res = p + 0xa4;
        Anim_LoadKeyframe(anim, res, sub_0822D604(0x1b), 1);
        cell = Hud_GetTilemapCell(0, 1, 6);
        cell[0] = 0xd000;
        cell[1] = 0xd000;
        p[0xafc] = 1;
    } else if (*(s16 *)(gUnk_02000710 + 0x612)) {
        *(u32 *)(p + 0xaa4) &= ~1;
        anim = p + 0xa9c;
        res = p + 0xa4;
        Anim_LoadKeyframe(anim, res, sub_0822D604(0x1c), 1);
        cell = Hud_GetTilemapCell(0, 1, 6);
        cell[0] = 0xd000;
        cell[1] = 0xd000;
        p[0xafc] = 1;
    } else if (p[0xafc]) {
        *(u32 *)(p + 0xaa4) |= 1;
        cell = Hud_GetTilemapCell(0, 1, 6);
        cell[0] = 0xd000;
        cell[1] = 0xd000;
        p[0xafc] = 0;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08165020);
#endif
