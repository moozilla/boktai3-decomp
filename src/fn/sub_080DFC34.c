#include "global.h"

// confidence=low: scene-actor animation state machine; picks a directional entry from the facing, dispatches on the facing quadrant, sets sequences and keyframes, and updates duration scales. Written as a goto-shaped draft that follows the ROM blocks.

// confidence=low: scene-actor animation state machine; picks a directional entry from the facing, dispatches on the facing quadrant, sets sequences and keyframes, and updates duration scales. Written as a goto-shaped draft that follows the ROM blocks.

// confidence=low: scene-actor animation state machine; picks a directional entry from the facing, dispatches on the facing quadrant, sets sequences and keyframes, and updates duration scales. Written as a goto-shaped draft that follows the ROM blocks.

// confidence=low: scene-actor animation state machine; picks a directional entry from the facing, dispatches on the facing quadrant, sets sequences and keyframes, and updates duration scales. Written as a goto-shaped draft that follows the ROM blocks.
#define DFC_B(o)  (*(u8 *)(self + (o)))
#define DFC_H(o)  (*(u16 *)(self + (o)))
#define DFC_W(o)  (*(u32 *)(self + (o)))
#define DFC_AW(o) (*(u32 *)(anim + (o)))

u32 Route_GetCurrentPointFacing(void *);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
s32 Div(s32, s32);

#ifdef NONMATCHING
// fuzzy: 12.0%
// scope shape 530 (differences once low registers are set aside)
// confidence=low: scene-actor animation state machine; picks a directional entry from the facing, dispatches on the facing quadrant, sets sequences and keyframes, and updates duration scales. Written as a goto-shaped draft that follows the ROM blocks.
void sub_080DFC34(u8 *self)
{
    u8 *anim;
    s32 trig, q, f, half, kf;
    u32 idx, r7, bits, pend, row, old;
    s32 d;

    anim = *(u8 **)(self + 4);
    if (DFC_B(0x2B2) != 0) {
        DFC_B(0x2B2) = 0;
        trig = 1;
    } else {
        trig = 0;
    }
    if (trig == 0)
        goto L_1A4;

    DFC_B(0x59) = ((((u8)DFC_B(0x59) + 0x60) >> 6) & 3) * 64 - 64;
    q = (((u8)DFC_B(0x59) + 0x60) >> 6) & 3;
    if (DFC_B(0x2AA) == 6)
        goto L_9C;
    if (DFC_H(0x2D4) & 0x480)
        goto L_9C;
    DFC_B(0x3CD) = (((u16)Route_GetCurrentPointFacing(*(void **)(self + 0x78)) + 0x70) >> 5) & 7;
    f = (s8)DFC_B(0x3CD);
    goto L_D6;
L_9C:
    f = (s8)DFC_B(0x3CD);
L_D6:
    half = (f < 0) ? -((-f) >> 1) : (f >> 1);
    if (q != half)
        goto L_D8A;

    DFC_B(0x2B0) = 1;
    q = 1;
    pend = 0;
    if (!(DFC_H(0x2D4) & 0x400)) {
        SceneActorAnimation_SelectDirectionalEntry(self, 0);
        goto L_E372;
    }
    SceneActorAnimation_SelectDirectionalEntry(self, 0);
    DFC_B(0x2A8) = 3;
    DFC_B(0x2A9) = 1;
    DFC_B(0x2AA) = pend;
    DFC_W(0x2AC) = 0;
    DFC_B(0x2B1) = 1;
    idx = DFC_W(0x5F0);
    DFC_B(0x2B2) = 1;
    DFC_B(0x2B0) = pend;
    DFC_B(0x2B5) = 0x1B;
    DFC_W(0x21C) = idx;
    DFC_H(0x2C8) = 0;
    DFC_W(0x114) &= ~1u;
    DFC_W(0x104) = 0;
    goto L_E372;

L_D8A:
    if (q == 1)
        goto L_E84;
    if (q > 1) {
        if (q == 2)
            goto L_F48;
        if (q == 3)
            goto L_E054;
        goto L_E19C;
    }
    if (q == 0)
        goto L_DA8;
    goto L_E19C;

L_DA8:
    if ((u32)(f - 2) > 3)
        goto L_DE00;
    idx = 0xD;
    r7 = 0;
    if (DFC_B(0x08) != 0)
        goto L_DDC;
    DFC_B(0x2B7) = 0xD;
    Anim_SetSequence(anim + 0x48, *(void **)(anim + 0x58), 0xD, 1, r7);
    goto L_E0D6;

L_DDC:
    idx = 0xE;
    if (DFC_B(0x2B7) == 0xE)
        goto L_E09E;
    goto L_E0AE;

L_DE00:
    idx = 0xD;
    r7 = 1;
    if (DFC_B(0x08) != 0)
        goto L_E24;
    DFC_B(0x2B7) = 0xD;
    row = 0;
    goto L_E0110;

L_E24:
    if (DFC_B(0x2B7) == 0xD && DFC_B(0x2B3) == 0 && (DFC_AW(0x28) & 4))
        goto L_E018E;
    DFC_B(0x2B7) = idx;
    kf = (r7 & 4) ? 6 : 4;
    Anim_SetKeyframes(anim + 0x20, anim, idx, 2, kf);
    if (r7 == 0)
        goto L_E0180;
    bits = DFC_AW(0x28) | 4;
    goto L_E0188;

L_E84:
    if ((u32)(f - 4) > 3)
        goto L_ED0;
    idx = 0xD;
    r7 = 0;
    if (DFC_B(0x08) != 0)
        goto L_EAC;
    DFC_B(0x2B7) = 0xD;
    row = 2;
    goto L_E0074;

L_EAC:
    idx = 0xF;
    if (DFC_B(0x2B7) == 0xF)
        goto L_E09E;
    goto L_E0AE;

L_ED0:
    idx = 0xD;
    r7 = 0;
    if (DFC_B(0x08) != 0)
        goto L_EF4;
    DFC_B(0x2B7) = 0xD;
    row = 3;
    goto L_E0074;

L_EF4:
    if (DFC_B(0x2B7) == 0x10 && DFC_B(0x2B3) == 0 && !(DFC_AW(0x28) & 4))
        goto L_E018E;
    DFC_B(0x2B7) = 0x10;
    kf = (r7 & 4) ? 6 : 4;
    Anim_SetKeyframes(anim + 0x20, anim, 0x10, 2, kf);
    goto L_E0180;

L_F48:
    if ((u32)(f - 1) <= 4)
        goto L_FFD0;
    idx = 0xD;
    r7 = 1;
    if (DFC_B(0x08) != 0)
        goto L_F70;
    DFC_B(0x2B7) = 0xD;
    row = 3;
    goto L_E0074;

L_F70:
    if (DFC_B(0x2B7) == 0x10 && DFC_B(0x2B3) == 0 && (DFC_AW(0x28) & 4))
        goto L_E0DA;
    DFC_B(0x2B7) = 0x10;
    kf = (r7 & 4) ? 6 : 4;
    Anim_SetKeyframes(anim + 0x20, anim, 0x10, 2, kf);
    if (r7 == 0)
        goto L_E0CC;
    bits = DFC_AW(0x28) | 4;
    goto L_E0D4;

L_FFD0:
    idx = 0xD;
    r7 = 1;
    if (DFC_B(0x08) != 0)
        goto L_FFF4;
    DFC_B(0x2B7) = 0xD;
    row = 2;
    goto L_E0110;

L_FFF4:
    if (DFC_B(0x2B7) == 0xF && DFC_B(0x2B3) == 0 && (DFC_AW(0x28) & 4))
        goto L_E018E;
    DFC_B(0x2B7) = 0xF;
    kf = (r7 & 4) ? 6 : 4;
    Anim_SetKeyframes(anim + 0x20, anim, 0xF, 2, kf);
    if (r7 == 0)
        goto L_E0180;
    bits = DFC_AW(0x28) | 4;
    goto L_E0188;

L_E054:
    if (f > 3)
        goto L_E0F4;
    idx = 0xD;
    r7 = 0;
    if (DFC_B(0x08) != 0)
        goto L_E088;
    DFC_B(0x2B7) = 0xD;
    row = 0;
    goto L_E0074;

L_E088:
    if (DFC_B(0x2B7) != 0xD)
        goto L_E0AE;
    if (DFC_B(0x2B3) == 0 && !(DFC_AW(0x28) & 4))
        goto L_E0DA;
    goto L_E0AE;

L_E0F4:
    idx = 0xD;
    r7 = 1;
    if (DFC_B(0x08) != 0)
        goto L_E124;
    DFC_B(0x2B7) = 0xD;
    row = 1;
    goto L_E0110;

L_E0110:
    Anim_SetSequence(anim + 0x48, *(void **)(anim + 0x58), 0xD, row, r7);
    goto L_E018A;

L_E124:
    if (DFC_B(0x2B7) == 0xE && DFC_B(0x2B3) == 0 && (DFC_AW(0x28) & 4))
        goto L_E018E;
    DFC_B(0x2B7) = 0xE;
    kf = (r7 & 4) ? 6 : 4;
    Anim_SetKeyframes(anim + 0x20, anim, 0xE, 2, kf);
    if (r7 == 0)
        goto L_E0180;
    bits = DFC_AW(0x28) | 4;
    goto L_E0188;

L_E0074:
    Anim_SetSequence(anim + 0x48, *(void **)(anim + 0x58), 0xD, row, r7);
    goto L_E0D6;

L_E0AE:
    DFC_B(0x2B7) = idx;
    kf = (r7 & 4) ? 6 : 4;
    Anim_SetKeyframes(anim + 0x20, anim, idx, 2, kf);
L_E0CC:
    bits = DFC_AW(0x28) & ~4;
L_E0D4:
    DFC_AW(0x28) = bits;
L_E0D6:
    DFC_B(0x2B3) = 0;
L_E0DA:
    DFC_W(0x2CC) |= 0x10000;
    goto L_E19C;

L_E09E:
    if (DFC_B(0x2B3) == 0 && !(DFC_AW(0x28) & 4))
        goto L_E0DA;
    goto L_E0AE;

L_E0180:
    bits = DFC_AW(0x28) & ~4;
L_E0188:
    DFC_AW(0x28) = bits;
L_E018A:
    DFC_B(0x2B3) = 0;
L_E018E:
    DFC_W(0x2CC) &= 0xFFFEFFFF;
L_E19C:
    DFC_B(0x2B3) = 0;

L_1A4:
    if (*(u16 *)(anim + 0x56) == 1) {
        if (DFC_W(0x2CC) & 0x10000)
            DFC_B(0x59) += 0x20;
        else
            DFC_B(0x59) -= 0x20;
    }

L_E01DC:
    old = DFC_H(0x2C2);
    if (DFC_H(0x2D4) & 0x80) {
        DFC_H(0x2C2) = Div(0x40, 3);
    }
    if (DFC_H(0x2D4) & 0xA02F) {
        DFC_H(0x2C2) = 0x40;
    } else if (DFC_H(0x2C2) == 0) {
        DFC_H(0x2C2) = 0x40;
    }
    if (DFC_B(0x08) == 0)
        Anim_SetDurationScale(anim + 0x48, DFC_H(0x2C2));
    else
        Anim_SetFrameDurationScale(anim + 0x20, DFC_H(0x2C2));
    DFC_H(0x2C2) = old;
    r7 = 0;
    pend = 0;
    if (DFC_B(0x2B3) == 0)
        goto L_E372;
    d = DFC_B(0x3CD) - (((DFC_B(0x59) + 0x70) >> 5) & 7);
    if (d < 0)
        d = -d;
    if (d > 0)
        goto L_E328;
    DFC_B(0x2B0) = 1;
    DFC_B(0x59) = ((((u8)DFC_B(0x59) + 0x60) >> 6) & 3) * 64 - 64;
    SceneActorAnimation_SelectDirectionalEntry(self, 0);
    if (!(DFC_H(0x2D4) & 0x400))
        goto L_E372;
    DFC_B(0x2A8) = 3;
    DFC_B(0x2A9) = 1;
    DFC_B(0x2AA) = pend;
    DFC_W(0x2AC) = r7;
    DFC_B(0x2B1) = 1;
    idx = DFC_W(0x5F0);
    DFC_B(0x2B2) = 1;
    DFC_B(0x2B0) = pend;
    DFC_B(0x2B5) = 0x1B;
    DFC_W(0x21C) = idx;
    DFC_H(0x2C8) = 0;
    goto L_E360;

L_E328:
    idx = DFC_W(0x224);
    DFC_B(0x2B2) = 1;
    DFC_B(0x2B0) = pend;
    DFC_B(0x2B5) = 2;
    DFC_W(0x21C) = idx;
    DFC_H(0x2C8) = 0;

L_E360:
    DFC_W(0x114) &= ~1u;
    DFC_W(0x104) = 0;

L_E372:
    return;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080DFC34);
#endif
