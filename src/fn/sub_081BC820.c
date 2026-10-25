#include "global.h"
#ifdef NONMATCHING
// fuzzy: 70.1%
// fix.py best (sweep after the inlined-insn fix)
// twin.py transplant from a matched near-clone
// 31 differing instruction runs; first diff at +0x4 (adds r7, r0, #0 vs adds r6, r0, #0)
void Bg_SetLayerMap(u32, u32, u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void DarkLoan_HideSprites(u8 *);
void Anim_SetKeyframes(u8 *, u8 *, u32, u32, u32);
void Anim_LoadKeyframe(u8 *, u8 *, u32, u32);
void DarkLoan_UpdateBalanceDigits(u8 *);
void StatGauge_DrawPreview(u8 *);
void DarkLoan_UpdateAdjustmentDigits(u8 *);
void AnimData_SelectPaletteBank(u8 *, u32);
void MessageWindow_ShowMessage(u32);
void DarkLoan_UpdateAdjustment(void);
void DarkLoan_SetState(u8 *, void (*)(void));
void StatGauge_BeginAdjustment(u8 *s)
{
    u32 zero;
    u32 latezero;
    u32 mask;
    u32 mask2;
    s8 mask3;
    u32 value;
    struct Word8 {
        u8 pad[8];
        u32 flags;
    };
    u8 *resource;
    u32 *p;
    u32 *q;
    u32 *initial;
    s32 i;
    s32 j;
    typeof(p) c1;
    Bg_SetLayerMap(0, *((u32 *)(s + 0x8E0)), 4);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    DarkLoan_HideSprites(s);
    value = *((u32 *)(s + 0x60));
    value &= (mask = ~1);
    *((u32 *)(s + 0x60)) = value;
    Anim_SetKeyframes(s + 0x58, resource = s + 0x18, 5, 1, zero);
    *((u32 *)(s + 0x240)) &= mask;
    Anim_LoadKeyframe(s + 0x238, resource, 0x12, 1);
    *((u16 *)(s + 0x8EE)) = zero;
    resource += 0x48;
    initial = (u32 *)(s + 0x2A0);
    i = 3;
    do {
        *initial &= mask;
        initial = (u32 *)(((u8 *)initial) + 0x60);
    } while ((--i) >= 0);
    DarkLoan_UpdateBalanceDigits(s);
    StatGauge_DrawPreview(s);
    *((u32 *)(s + 0x5A0)) &= (mask2 = ~1);
    q = (u32 *)(resource + 0x5A0);
    p = (u32 *)(resource + 0x3C0);
    c1 = p;
    j = 3;
    do {
        *c1 &= mask2;
        *q &= mask2;
        q = (u32 *)(((u8 *)q) + 0x60);
        p = (u32 *)(((u8 *)c1) + 0x60);
    } while ((--j) >= 0);
    latezero = 0;
    DarkLoan_UpdateAdjustmentDigits(s);
    {
        u16 *d1 = (u16 *)(s + 0x8F8);
        d1[8] = latezero;
    }
    *((u16 *)(s + 0x90A)) = latezero;
    ((struct Word8 *)(s + 0x778))->flags &= (mask3 = ~1);
    *((u32 *)(s + 0x898)) &= mask3;
    AnimData_SelectPaletteBank(s + 0x8C4, 0x1B4);
    ((struct Word8 *)(s + 0x7D8))->flags &= mask3;
    ((struct Word8 *)(s + 0x838))->flags &= mask3;
    MessageWindow_ShowMessage(4);
    *((u16 *)(s + 0x8FA)) = latezero;
    *((u16 *)(s + 0x8FC)) = latezero;
    *((u16 *)(s + 0x8FE)) = latezero;
    *((u16 *)(s + 0x900)) = latezero;
    *((u16 *)(s + 0x8F8)) = latezero;
    DarkLoan_SetState(s, DarkLoan_UpdateAdjustment);
}

#else
INCLUDE_ASM("asm/nonmatching", StatGauge_BeginAdjustment);
#endif
