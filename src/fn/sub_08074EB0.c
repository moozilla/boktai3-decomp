#include "global.h"

extern u16 *gUnk_030052F4_ptr;

// confidence=low: per-frame update of a scene actor: animation step, movement and height, with sink and effect handling.

// confidence=low: per-frame update of a scene actor: animation step, movement and height, with sink and effect handling.
s32 Anim_Step(void *);
void SceneActorMovement_UpdateAirborne(void *);
void Obj_StepPosition(void *);
void SceneActorMovement_UpdateHeight(void *);
s32 TerrainModifiers_FindCellOverride(u32, u32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
s32 Math_Atan2(s32, s32);
void RepeatedEffect_RequestNearby(void *);
void SceneActorReaction_SinkAndRemove(void *);

#ifdef NONMATCHING
// fuzzy: 29.9%
// scope shape 374 (differences once low registers are set aside)
// confidence=low: per-frame update of a scene actor: animation step, movement and height, with sink and effect handling.
void sub_08074EB0(struct SceneActorRegistryActor *arg0)
{
    u8 *p = (u8 *)arg0;
    u8 *a;
    u8 *b;
    u8 *c;
    u8 *m8;
    u8 *s;
    u16 *tab;
    u16 *sp14;
    u16 *slot;
    u16 vec[3];
    u16 dif[3];
    s32 f;
    s32 r2;
    s32 r3;
    s32 r4;
    s32 r6;
    s32 st;
    s32 cnt;

    *(u8 *)(p + 0x2B4) = 1;
    if (*(u32 *)(p + 0x2CC) & 1) {
        *(u16 *)(p + 0x2D4) |= 0x40;
    }
    if (*(u8 *)(p + 0x2B3) == 1)
        goto L_0802502A;
    if (*(u8 *)(p + 8) != 0)
        goto L_08075008;

    a = (u8 *)*(u32 *)(p + 4);
    b = a + 0x48;
    tab = (u16 *)(*(u32 *)(a + 0x48) + *(u16 *)(b + 8) * 2);
    *(u16 *)(a + 0x10) = *tab >> 6;
    if ((b[4] & 1) != ((*tab >> 4) & 1)) {
        *(u32 *)a |= 4;
    } else {
        *(u32 *)a &= ~4;
    }
    if ((b[4] & 2) != ((*tab >> 4) & 2)) {
        *(u32 *)a |= 8;
    } else {
        *(u32 *)a &= ~8;
    }

    cnt = (u16)(*(u16 *)(b + 0xE) += 1);
    r2 = 0;
    if (cnt < b[7])
        goto L_0807502B_ff8;
    *(u16 *)(b + 0xE) = 0;
    r3 = b[4];
    if (!(r3 & 4)) {
        cnt = *(u16 *)(b + 8) + 1;
        *(u16 *)(b + 8) = cnt;
        if ((u16)cnt < b[5]) {
            r2 = 0;
        } else {
            if (!(r3 & 8)) {
                *(u16 *)(b + 8) = 0;
            } else {
                *(u16 *)(b + 0xE) = b[7];
                *(u16 *)(b + 8) = b[5] - 1;
            }
            r2 = 1;
        }
    } else if (*(u16 *)(b + 8) != 0) {
        *(u16 *)(b + 8) -= 1;
        r2 = 0;
    } else if (!(r3 & 8)) {
        *(u16 *)(b + 8) = b[5] - 1;
        r2 = 1;
    } else {
        *(u16 *)(b + 0xE) = b[7];
        r2 = 1;
    }
    tab = (u16 *)(*(u32 *)b + *(u16 *)(b + 8) * 2);
    b[6] = *tab & 0xF;
    cnt = (b[6] * *(u16 *)(b + 0xC)) >> 6;
    b[7] = cnt;
    if ((cnt << 24) == 0)
        b[7] = 1;
    goto L_0807502B_set;

L_0807502B_ff8:
    r2 = 0;
L_0807502B_set:
    *(u8 *)(p + 0x2B3) = r2;
    goto L_0802502A;

L_08075008:
    s = (u8 *)(*(u32 *)(p + 4) + 0x20);
    r4 = Anim_Step(s);
    if ((s[0x1D] & 1) != 0 || r4 != 0)
        *(u8 *)(p + 0x2B3) = 1;

L_0802502A:
    if (arg0 != 0) {
        if (*(u16 *)(p + 0x2D4) & 0x40)
            r4 = 1;
        else if (*(u32 *)(p + 0x2D0) & 0x20000)
            r4 = 1;
        else if (*(s16 *)(p + 0x9C) <= 0)
            r4 = 1;
        else
            r4 = 0;
    } else {
        r4 = 0;
    }
    if (r4 == 0)
        goto L_08075068;
    return;

L_08075068:
    SceneActorMovement_UpdateAirborne(p);
    if (*(u16 *)(p + 0x2C0) & 0x20) {
        c = 0;
    } else if (*(u32 *)(p + 0x2CC) & 0x2000) {
        c = p + 0x40;
    } else {
        c = 0;
    }
    *(u8 **)(p + 0x6C) = c;
    m8 = p + 0x5C;
    if (*(s16 *)(p + 0x64) != 0)
        goto L_080750BC;
    if (*(s16 *)(p + 0x68) == 0)
        goto L_080750D8;

L_080750BC:
    *(u16 *)(p + 0x2D6) |= 0x2000;
    sp14 = (u16 *)(p + 0x2D6);
    goto L_08075116;

L_080750D8:
    sp14 = (u16 *)(p + 0x2D6);
    if (*(s16 *)(p + 0x2FC) == *(s16 *)m8)
        goto L_08075116;
    if (*(s16 *)(p + 0x300) == *(s16 *)(p + 0x60))
        goto L_08075116;
    *(u16 *)(p + 0x2D6) |= 0x2000;

L_08075116:
    if (*sp14 & 0x2000)
        Obj_StepPosition(p + 0x50);
    *(u16 *)(p + 0x2F6) = *(u16 *)(p + 0x2F4);
    if ((*(s16 *)m8 >> 8) < 0)
        goto L_0807516A;
    if ((*(s16 *)(m8 + 4) >> 8) < 0)
        goto L_0807516A;
    if ((u32)(*(s16 *)m8 >> 8) >= *(u32 *)0x030052F8)
        goto L_0807516A;
    if ((u32)(*(s16 *)(m8 + 4) >> 8) >= *(u32 *)0x030052FC)
        goto L_0807516A;
    r4 = (*(s16 *)(m8 + 4) >> 8) << 1;
    f = *(u16 *)(*(u32 *)0x030052F4 + 0x24 + r4);
    r4 = f + (*(s16 *)m8 >> 8);
    goto L_0807518A;

L_0807516A:
    r4 = 0;

L_0807518A:
    *(u16 *)(p + 0x2F4) = r4;
    SceneActorMovement_UpdateHeight(p);
    if (*sp14 & 0x2000)
        f = 1;
    else
        f = 0;
    if (f == 0)
        goto L_080754C8;
    st = *(u8 *)(p + 0x98);
    if ((u32)(st - 0x11) <= 3 || st == 0x17)
        goto L_080754C8;

    slot = (u16 *)(p + 0x2F4);
    r6 = *slot;
    f = TerrainModifiers_FindCellOverride(r6, 1);
    if (f != 0) {
        c = (u8 *)(f + 4);
    } else {
        c = (u8 *)(*(u32 *)(*(u32 *)0x030052F4 + 4) + (r6 << 2) + 0xC);
    }
    f = *(u16 *)(c + 2);
    if (*(u16 *)(p + 0x2F4) == *(u16 *)(p + 0x2F6))
        goto L_08075458;
    if (!(f & 0x40))
        goto L_08075338;
    if ((*(s16 *)(p + 0x5E) + *(s16 *)(p + 0x2EA)) <= *(u16 *)(p + 0x2BE))
        goto L_08075234;
    goto L_08075338;

L_08075234:
    if (arg0 == 0) {
        r4 = 0;
    } else if (*(u16 *)(p + 0x2D4) & 0x40) {
        r4 = 1;
    } else if (*(u32 *)(p + 0x2D0) & 0x20000) {
        r4 = 1;
    } else if (*(s16 *)(p + 0x9C) > 0) {
        r4 = 0;
    } else {
        r4 = 1;
    }
    if (r4 != 0)
        goto L_080754C8;
    r3 = *(u32 *)(p + 0x238);
    if (r3 != 0)
        goto L_080752B0;
    r3 = (s32)SceneActorReaction_SinkAndRemove;
    r2 = 0x16;
    *(u8 *)(p + 0x2B2) = 1;
    *(u8 *)(p + 0x2B0) = r4;
    r6 = 0x2B5;
    goto L_080752C4;

L_080752B0:
    r2 = 0x16;
    *(u8 *)(p + 0x2B2) = 1;
    *(u8 *)(p + 0x2B0) = r4;
    r6 = 0x2B5;

L_080752C4:
    *(u8 *)(p + r6) = r2;
    *(u32 *)(p + 0x21C) = r3;
    *(u16 *)(p + 0x2C8) = r4;
    *(u32 *)(p + 0x114) &= ~1;
    *(u32 *)(p + 0x104) = r4;
    *(u8 *)(p + 0x2A8) = 3;
    *(u8 *)(p + 0x2A9) = 0;
    *(u8 *)(p + 0x2AA) = 0;
    *(u32 *)(p + 0x2AC) = 0;
    *(u8 *)(p + 0x2B1) = 1;
    *(u32 *)(p + 0x2D0) |= 0x20000;
    goto L_080754C8;

L_08075338:
    if (!(f & 0x80))
        goto L_08075458;
    if (!(*(u32 *)(*(u32 *)0x02000710 + 0x868) & 0x10))
        goto L_08075458;

    /* 0x08075368: rotate target to the arrival cell */
    {
        u8 *sp10 = p + 0x134;
        u16 *ptr = (u16 *)(p + 0x2F4);
        r6 = *ptr;
        r4 = Mod(r6, *(s16 *)(*(u32 *)(*(u32 *)0x030052F4 + 4) + 4));
        r3 = Div(r6, *(s16 *)(*(u32 *)(*(u32 *)0x030052F4 + 4) + 4));
        vec[0] = (r4 << 8) + 0x80;
        f = TerrainModifiers_FindCellOverride(r6, 1);
        if (f != 0) {
            c = (u8 *)(f + 4);
        } else {
            c = (u8 *)(*(u32 *)(*(u32 *)0x030052F4 + 4) + (r6 << 2) + 0xC);
        }
        vec[1] = (c[0] & 0xF) << 8;
        vec[2] = (r3 << 8) + 0x80;
        dif[0] = vec[0] - *(u16 *)m8;
        dif[1] = vec[1] - *(u16 *)(m8 + 2);
        dif[2] = vec[2] - *(u16 *)(m8 + 4);
        f = Math_Atan2((s16)dif[0], (s16)dif[2]);
        *(u8 *)(p + 0x2B6) = f >> 5;
        *(u32 *)(p + 0x2E0) = 0x20;
        *(u16 *)(sp10 + 0x3E) = *(s16 *)(p + 0x9C);
        *(u16 *)(p + 0x2E4) = *(s16 *)(p + 0x9C);
        *(u16 *)(sp10 + 0x40) = 0x28;
        *(u32 *)(p + 0x2D8) = 0x200;
        *(u16 *)(p + 0x2E6) = 1;
        *(u32 *)(p + 0x2CC) |= 0x2000;
    }
    goto L_080754C8;

L_08075458:
    if (st != 3)
        goto L_0807548C;
    if (!(*(u16 *)(p + 0x2CA) & 1))
        goto L_0807548C;
    if (!(*(u16 *)(p + 0x2D4) & 0x100))
        goto L_0807548C;
    *(u32 *)(p + 0x2D0) |= 0x10000;

L_0807548C:
    if (*(u8 *)(p + 8) == 0)
        f = *(u16 *)(*(u32 *)(p + 4) + 0x48 + 0xE);
    else
        f = *(u16 *)(*(u32 *)(p + 4) + 0x2C);
    if (f != 0)
        goto L_080754C8;
    if (!(*(u32 *)(p + 0x2D0) & 0x10000))
        goto L_080754C8;
    RepeatedEffect_RequestNearby(m8);
    *(u32 *)(p + 0x2D0) &= 0xFFFEFFFF;

L_080754C8:
    *sp14 &= 0xDFFF;
    *(u16 *)(p + 0x2CA) += 1;
    *(u32 *)(p + 0x2FC) = *(u32 *)(p + 0x5C);
    *(u32 *)(p + 0x300) = *(u32 *)(p + 0x60);
    return;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08074EB0);
#endif
