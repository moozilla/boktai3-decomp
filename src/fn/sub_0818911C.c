#include "global.h"

void ActorController_SetBehavior(void *, s32);
s32 Math_Atan2(s32, s32);

struct H10bArmor {
    u8 unknown0[0x48];
    s16 x48;
    u8 unknown4A[2];
    s16 y4C;
    u8 unknown4E[0x1C8 - 0x4E];
    u32 word1C8;
    u8 unknown1CC[0x21C - 0x1CC];
    u32 word21C;
    u8 unknown220[0xB26 - 0x220];
    u16 field_B26;
    u8 unknownB28[0xB32 - 0xB28];
    u16 flagsB32;
    u8 unknownB34[0xB38 - 0xB34];
    u8 field_B38;
    u8 field_B39;
    u8 unknownB3A[0xB3C - 0xB3A];
    u8 field_B3C;
    u8 field_B3D;
    u8 unknownB3E[0x1120 - 0xB3E];
    u8 state1120;
    u8 flag1121;
    s16 t1122;
    s16 t1124;
    u16 f1126;
    u16 f1128;
    s16 t112A;
    u16 f112C;
};

struct H10bPlayer {
    u8 unknown0[0x30];
    s16 x30;
    u8 unknown32[2];
    s16 y34;
};

#ifdef NONMATCHING
// fuzzy: 43.6%
// scope shape 311 (differences once low registers are set aside)
// confidence=low: armor boss timers and the chase/attack state machine, player-relative
void sub_0818911C(void *arg0)
{
    struct H10bArmor *a = arg0;
    struct H10bPlayer *player;
    s32 px, py, ax, ay;
    s32 dx, dy, dxR, dyR, absX, absY, sum;

    if (a->t1122 > 0) {
        a->t1122--;
    }
    if (a->t112A > 0) {
        a->t112A--;
        if (a->t112A <= 0) {
            a->state1120 = 4;
            a->flag1121 = 0;
            a->t1122 = 0x12C;
            a->field_B26 = a->f112C;
            a->f1128 = 0;
            a->word21C &= 0xFFBFFFF7;
            a->word1C8 &= 0xFFBFFFF7;
        }
    }
    if (a->t1124 > 0) {
        a->t1124--;
    }

    if (a->field_B3D != 0xFF && a->field_B3C != 2 && a->field_B3C != 0x26 && a->field_B3C != 0x25 &&
        a->field_B3C != 0x27) {
        return;
    }

    player = *(struct H10bPlayer **)0x02000710;
    px = player->x30;
    ax = a->x48;
    dx = px - ax;
    py = player->y34;
    ay = a->y4C;
    dy = py - ay;
    dxR = (dx + 0x80) >> 8;
    dyR = (dy + 0x80) >> 8;
    absX = dxR < 0 ? -dxR : dxR;
    absY = dyR < 0 ? -dyR : dyR;

    if (a->flagsB32 & 1) {
        if (a->state1120 > 4) {
            return;
        }
        switch (a->state1120) {
            case 0:
                if (absX + absY <= 4) {
                    ActorController_SetBehavior(a, a->flag1121 ? 0x27 : 0x25);
                    a->state1120 = 1;
                    a->t1122 = 0x168;
                } else if (a->flag1121 == 0 && a->t1124 > 0) {
                    ActorController_SetBehavior(a, 2);
                } else {
                    ActorController_SetBehavior(a, 0x26);
                }
                return;
            case 1:
                if (a->field_B3C == 3) {
                    ActorController_SetBehavior(a, 0x1e);
                    return;
                }
                if (absX + absY > 5) {
                    if (a->flag1121 == 0 && a->t1124 > 0) {
                        ActorController_SetBehavior(a, 2);
                    } else {
                        ActorController_SetBehavior(a, 0x26);
                    }
                    a->state1120 = 0;
                    return;
                }
                {
                    s32 dir;
                    dir = (Math_Atan2(dx, dy) + 0x20) & 0xC0;
                    if (a->field_B39 == dir && sum <= 1) {
                        ActorController_SetBehavior(a, 3);
                        return;
                    }
                    if (a->t1122 > 0 || (dxR != 0 && dyR != 0)) {
                        if (a->flag1121 == 0 && a->t1124 > 0) {
                            ActorController_SetBehavior(a, 0x25);
                        } else {
                            ActorController_SetBehavior(a, 0x27);
                        }
                    } else {
                        if (a->flag1121 != 0) {
                            ActorController_SetBehavior(a, 0x24);
                        } else {
                            ActorController_SetBehavior(a, 0x18);
                        }
                        a->t1122 = 0x168;
                    }
                }
                return;
            case 2:
                ActorController_SetBehavior(a, 3);
                a->state1120 = 1;
                a->t1122 = 0x168;
                return;
            case 3:
                if ((u8)(a->field_B3C - 10) > 1 && a->field_B3C != 0xd) {
                    ActorController_SetBehavior(a, 0xb);
                    return;
                }
                if (a->t1122 > 0) {
                    if (a->flag1121 != 0) {
                        ActorController_SetBehavior(a, 0xc);
                        a->state1120 = 0;
                        a->f1126 = 0;
                    }
                } else {
                    ActorController_SetBehavior(a, 0xc);
                    a->state1120 = 0;
                    a->f1126 = 0;
                }
                return;
            case 4:
                if (a->t1122 > 0) {
                    ActorController_SetBehavior(a, 0x1d);
                    return;
                }
                ActorController_SetBehavior(a, 2);
                a->state1120 = 0;
                a->flag1121 = 0;
                a->f1128 = 0;
                return;
        }
        return;
    }

    if (a->field_B38 == 0xE) {
        ActorController_SetBehavior(a, 0x29);
        return;
    }
    switch (a->state1120) {
        case 0:
            sum = absX + absY;
            if (sum <= 4) {
                ActorController_SetBehavior(a, a->flag1121 ? 0x27 : 0x25);
                a->state1120 = 1;
                a->t1122 = 0x168;
            } else if (a->flag1121 == 0 && a->t1124 > 0) {
                ActorController_SetBehavior(a, 2);
            } else {
                ActorController_SetBehavior(a, 0x26);
            }
            return;
        case 1:
            if (a->field_B3C == 3) {
                ActorController_SetBehavior(a, 0x1e);
                return;
            }
            if (absX + absY > 5) {
                if (a->flag1121 == 0 && a->t1124 > 0) {
                    ActorController_SetBehavior(a, 2);
                } else {
                    ActorController_SetBehavior(a, 0x26);
                }
                a->state1120 = 0;
                return;
            }
            {
                s32 dir;
                dir = (Math_Atan2(dx, dy) + 0x20) & 0xC0;
                if (a->field_B39 == dir && sum <= 1) {
                    ActorController_SetBehavior(a, 3);
                    return;
                }
                if (a->t1122 > 0 || (dxR != 0 && dyR != 0)) {
                    if (a->flag1121 == 0 && a->t1124 > 0) {
                        ActorController_SetBehavior(a, 0x25);
                    } else {
                        ActorController_SetBehavior(a, 0x27);
                    }
                } else {
                    if (a->flag1121 != 0) {
                        ActorController_SetBehavior(a, 0x24);
                    } else {
                        ActorController_SetBehavior(a, 0x18);
                    }
                    a->t1122 = 0x168;
                }
            }
            return;
        case 2:
            ActorController_SetBehavior(a, 3);
            a->state1120 = 1;
            a->t1122 = 0x168;
            return;
        case 4:
            if (a->t1122 > 0) {
                ActorController_SetBehavior(a, 0x1d);
                return;
            }
            ActorController_SetBehavior(a, 2);
            a->state1120 = 0;
            a->flag1121 = 0;
            a->f1128 = 0;
            return;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0818911C);
#endif
