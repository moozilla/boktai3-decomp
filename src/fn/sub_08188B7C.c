#include "global.h"

// confidence=low: armor boss timers and the chase/attack state machine, player-relative

// confidence=low: armor boss timers and the chase/attack state machine, player-relative

// confidence=low: armor boss timers and the chase/attack state machine, player-relative

// confidence=low: armor boss timers and the chase/attack state machine, player-relative
struct Player08188B7C {
    u8 unk00[0x30];
    s16 x30;
    u8 unk32[2];
    s16 y34;
};

struct Armor08188B7C {
    u8 unk00[0x18];
    u16 flags18;
    u8 unk1A[0x48 - 0x1A];
    s16 x48;
    u8 unk4A[2];
    s16 y4C;
    u8 unk4E[0x1C8 - 0x4E];
    u32 word1C8;
    u8 unk1CC[0x21C - 0x1CC];
    u32 word21C;
    u8 unk220[0x274 - 0x220];
    u16 f274;
    u8 unk276[0xB26 - 0x276];
    u16 fB26;
    u16 fB28;
    u8 unkB2A[0xB32 - 0xB2A];
    u16 flagsB32;
    u8 unkB34[0xB38 - 0xB34];
    u8 fB38;
    u8 fB39;
    u8 unkB3A[0xB3C - 0xB3A];
    u8 fB3C;
    u8 fB3D;
    u8 unkB3E[0x1120 - 0xB3E];
    u8 state1120;
    u8 flag1121;
    s16 t1122;
    s16 t1124;
    s16 t1126;
    u16 f1128;
    s16 t112A;
    u16 f112C;
    u16 f112E;
};

void ActorController_SetBehavior(void *, s32);
void ActorController_SetBehaviorIfFinished(void *, s32);
s32 Math_Atan2(s32, s32);

#ifdef NONMATCHING
// fuzzy: 45.2%
// scope shape 201 (differences once low registers are set aside)
// confidence=low: armor boss timers and the chase/attack state machine, player-relative
void sub_08188B7C(void *arg0)
{
    struct Armor08188B7C *a = arg0;
    struct Player08188B7C *player;
    s32 dx, dy, dxR, dyR, absX, absY, sum, dir;

    if (a->t1122 > 0)
        a->t1122--;
    if (a->t1124 > 0)
        a->t1124--;
    if (a->t1126 > 0)
        a->t1126--;
    if (a->fB3D != 0xFF && a->fB3C != 2 && a->fB3C != 0x1a && a->fB3C != 0x1b && a->fB3C != 0x1c)
        return;

    player = *(struct Player08188B7C **)0x02000710;
    dx = player->x30 - a->x48;
    dy = player->y34 - a->y4C;
    dxR = (dx + 0x80) >> 8;
    dyR = (dy + 0x80) >> 8;
    absX = dxR < 0 ? -dxR : dxR;
    absY = dyR < 0 ? -dyR : dyR;
    sum = absX + absY;

    if (a->flagsB32 & 1) {
        if (a->state1120 > 5)
            return;
        switch (a->state1120) {
            case 0:
                if (a->flagsB32 & 2) {
                    ActorController_SetBehavior(a, 0xC);
                    return;
                }
                if ((dxR == 0 || dyR == 0) && a->fB3C == 2 && (a->flags18 & 4)) {
                    ActorController_SetBehavior(a, 0x18);
                    return;
                }
                ActorController_SetBehaviorIfFinished(a, 2);
                return;
            case 1:
                if (a->t1122 <= 0) {
                    ActorController_SetBehavior(a, 2);
                    a->state1120 = 0;
                    a->f1128 = 0;
                    a->t112A = 0;
                    return;
                }
                if (a->fB3C == 3) {
                    ActorController_SetBehavior(a, 0xA);
                    a->t1126 = 0x78;
                    return;
                }
                if (a->fB3C == 0xF) {
                    ActorController_SetBehavior(a, 0xA);
                    a->t1126 = 0x78;
                    return;
                }
                if ((u8)(a->fB3C - 10) <= 1 || a->fB3C == 0xD) {
                    if (a->t1126 > 0)
                        return;
                }
                dir = (Math_Atan2(player->x30 - a->x48, player->y34 - a->y4C) + 0x20) & 0xC0;
                if (a->fB39 == dir && sum <= 2) {
                    ActorController_SetBehavior(a, 3);
                    return;
                }
                if (a->t1124 > 0)
                    ActorController_SetBehaviorIfFinished(a, 0x1b);
                else
                    ActorController_SetBehaviorIfFinished(a, 0x1c);
                return;
            case 2:
                if (a->t1122 > 0) {
                    ActorController_SetBehaviorIfFinished(a, 5);
                    return;
                }
                ActorController_SetBehavior(a, 6);
                a->state1120 = 0;
                return;
            case 3:
                if (a->t1124 <= 0)
                    goto state5;
                if (a->flagsB32 & 2) {
                    ActorController_SetBehavior(a, 0xC);
                    return;
                }
                if ((dxR == 0 || dyR == 0) && a->fB3C == 0x1a && (a->flags18 & 4)) {
                    ActorController_SetBehavior(a, 0x19);
                    return;
                }
                ActorController_SetBehaviorIfFinished(a, 0x1a);
                return;
            case 4:
                if (a->t1122 > 0) {
                    ActorController_SetBehaviorIfFinished(a, 5);
                    return;
                }
                ActorController_SetBehavior(a, 6);
                a->state1120 = 3;
                return;
            case 5:
                if (a->t1122 > 0) {
                    ActorController_SetBehavior(a, 0x1d);
                    return;
                }
                a->state1120 = 0;
                return;
        }
        return;
    }

    if (a->fB38 == 0xD) {
        ActorController_SetBehavior(a, 0x29);
        return;
    }
    if (a->state1120 > 5)
        return;
    switch (a->state1120) {
        case 0:
            if (a->flagsB32 & 2) {
                ActorController_SetBehavior(a, 0xC);
                return;
            }
            if ((dxR == 0 || dyR == 0) && a->fB3C == 2 && (a->flags18 & 4)) {
                ActorController_SetBehavior(a, 0x18);
                return;
            }
            ActorController_SetBehaviorIfFinished(a, 2);
            return;
        case 1:
            if (a->t1122 <= 0) {
                a->state1120 = 0;
                a->f1128 = 0;
                a->t112A = 0;
                return;
            }
            if (a->fB3C == 3) {
                ActorController_SetBehavior(a, 0x1e);
                return;
            }
            dir = (Math_Atan2(player->x30 - a->x48, player->y34 - a->y4C) + 0x20) & 0xC0;
            if (a->fB39 == dir) {
                if (sum <= 2) {
                    ActorController_SetBehavior(a, 3);
                    return;
                }
                if (sum > 4) {
                    ActorController_SetBehavior(a, 9);
                    return;
                }
            }
            if (a->t1124 > 0)
                ActorController_SetBehaviorIfFinished(a, 0x1b);
            else
                ActorController_SetBehaviorIfFinished(a, 0x1c);
            return;
        case 2:
            if (a->t1122 > 0) {
                ActorController_SetBehaviorIfFinished(a, 5);
                return;
            }
            ActorController_SetBehavior(a, 6);
            a->state1120 = 0;
            return;
        case 3:
            if (a->t1124 > 0) {
                if ((dxR == 0 || dyR == 0) && a->fB3C == 0x1a && (a->flags18 & 4)) {
                    ActorController_SetBehavior(a, 0x19);
                    return;
                }
                ActorController_SetBehaviorIfFinished(a, 0x1a);
                return;
            }
        state5:
            a->state1120 = 5;
            a->t1122 = 0x12C;
            a->fB26 = a->f112C;
            a->fB28 = a->f112E;
            a->f274 = a->f112E;
            a->word21C &= 0xFFDFFFFB;
            a->word1C8 &= 0xFFDFFFFB;
            return;
        case 4:
            if (a->t1122 > 0) {
                ActorController_SetBehaviorIfFinished(a, 5);
                return;
            }
            ActorController_SetBehavior(a, 6);
            a->state1120 = 3;
            return;
        case 5:
            if (a->t1122 > 0) {
                ActorController_SetBehavior(a, 0x1d);
                return;
            }
            a->state1120 = 0;
            return;
    }
    return;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08188B7C);
#endif
