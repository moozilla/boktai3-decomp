#include "global.h"
#include "terrain_cell_probe.h"
#include "ram.h"

struct BarrierSpr {
    u32 flags;
    u32 pad[3];
    u16 h;
};
#define SPR ((struct BarrierSpr *)(b + 0xF4))

struct BarrierGame {
    u8 f00[0x77C];
    s32 duration;
};

void AnimData_SelectPaletteBank(void *, s32);
s32 Collider_Refresh(void *);
void PlayerBarrier_DecrementDuration(void *);

s32 PlayerBarrier_Update(u8 *b)
{
    s32 st;
    s32 *pst, *q;
    if ((gSceneEffectPalettePhase & 3) == 0) {
        if (TerrainModifiers_FindCellOverride(cellIndexAt((struct Pos16 *)(b + 0x70)), 0))
            *(s32 *)(b + 0xB8) = 0;
    }
    pst = (s32 *)(b + 0xB8);
    st = *pst;
    q = pst;
    if (st == 3) {
        u32 *cnt;
        AnimData_SelectPaletteBank(pst + 1, 0x64);
        AnimData_SelectPaletteBank(b + 0xD8, 0x223);
        SPR->h = 5;
        cnt = (u32 *)(b + 0x14C);
        if (*cnt > 5) {
            *(u32 *)(b + 0x120) &= ~1;
            Collider_Refresh(b + 0x64);
        }
        PlayerBarrier_DecrementDuration(b);
        if (((struct BarrierGame *)gGameState)->duration <= 0)
            *q = 2;
        (*cnt)++;
    } else if (st == 1) {
        u32 c;
        u32 off = 0x14C;
        c = ++*(u32 *)(b + off);
        if (c <= 29) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x222);
            SPR->h = 0;
        } else if (c <= 59) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x222);
            SPR->h = st;
        } else if (c <= 65) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x221);
            SPR->h = st;
        } else if (c <= 71) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x220);
            SPR->h = st;
        } else if (c <= 91) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x21F);
            SPR->h = st;
        } else if (c <= 97) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x21F);
            SPR->h = 2;
        } else if (c <= 103) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x21F);
            SPR->h = 3;
        } else if (c <= 109) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x21F);
            SPR->h = 4;
        } else if (c <= 129) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x21F);
            SPR->h = 5;
        } else {
            *(s32 *)(b + off) = 0;
            *q = 2;
        }
        *(u32 *)(b + 0x120) |= 1;
        SPR->flags &= ~1;
    } else if (st == 2) {
        u32 c;
        u32 *cnt = (u32 *)(b + 0x14C);
        c = ++*cnt;
        if (c <= 19) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x21F);
        } else if (c <= 39) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x220);
        } else if (c <= 59) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x221);
        } else if (c <= 79) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x222);
        } else if (c <= 99) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x221);
        } else if (c <= 119) {
            AnimData_SelectPaletteBank(b + 0xD8, 0x220);
        } else {
            *cnt = 0;
        }
        SPR->h = 5;
        *(u32 *)(b + 0x120) |= 1;
    } else {
        *(s32 *)(b + 0x14C) = 0;
        *(u32 *)(b + 0x120) |= 1;
    }
    return 0;
}
