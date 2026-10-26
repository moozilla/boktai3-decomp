#include "global.h"
#include "ram.h"

struct Sub {
    u8 pad[0x38];
    u32 flags;
    u8 pad2[0x54 - 0x3c];
};

struct Obj {
    u8 pad0[0x190];
    struct Sub sub[2];
    u8 pad1[0xB1C - 0x238];
    u16 hB1C;
    u8 pad2[0xB26 - 0xB1E];
    u16 hB26;
    u8 pad3[0xB32 - 0xB28];
    u16 keys;
    u8 pad4[0xB38 - 0xB34];
    u8 mode;
    u8 pad5[0x1120 - 0xB39];
    u8 state;
    u8 b1121;
    u16 timer;
    u16 acc;
    u16 h1126;
    u16 h1128;
    u16 h112A;
    u16 h112C;
};

extern u16 gUnk_086141BC[];
extern void Armor_UpdateContactBoost(struct Obj *, s32, u32);
extern void Armor_UpdateAccumulatedBehavior(struct Obj *, s32, u32);

static inline void clearFlags(struct Sub *p, u32 mask)
{
    p->flags &= ~mask;
}

void Armor_UpdateBehaviorAfterContact(struct Obj *p, s32 delta, u32 flags, s32 flag)
{
    s32 k2 = 0;
    s32 v;
    u16 *q;
    s32 sum;
    switch (p->mode) {
        case 0:
            if (flag) {
                p->state = 0;
                p->timer = 0;
            } else {
                switch (p->state) {
                    case 0:
                        p->state = 1;
                        p->timer = 0xB4;
                        break;
                    case 1:
                        p->timer = 0xB4;
                        break;
                }
            }
            break;
        case 1:
            if (flag) {
                p->state = 2;
                p->timer = 0x5A;
            } else if (p->keys & 1) {
                switch (p->state) {
                    case 0:
                        p->state = 1;
                        p->timer = 0xB4;
                        break;
                    case 1:
                        if (delta > 0) {
                            p->state = 2;
                            p->timer = 0x5A;
                        } else {
                            p->timer = 0xB4;
                        }
                        break;
                }
            } else {
                gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
                v = gUnk_086141BC[gEffectRandomTableIndex];
                if (v > 0x3FFF) {
                    p->state = 2;
                    p->timer = 0x5A;
                } else {
                    p->state = 3;
                }
            }
            break;
        case 2:
        case 3:
            q = &p->acc;
            sum = *q + delta;
            *q = sum;
            if (flag) {
                p->state = 1;
                p->timer = 0x5A;
                {
                    u32 zero = 0;
                    *q = k2;
                }
            } else if (p->keys & 1) {
                if ((s16)sum > 400) {
                    p->state = 2;
                    p->timer = 0xB4;
                    *q = flag;
                }
            } else if ((s16)sum > 400) {
                gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
                v = gUnk_086141BC[gEffectRandomTableIndex];
                if (v > 0x7FFF) {
                    p->state = 2;
                } else {
                    p->state = 1;
                    p->timer = 0x5A;
                }
                *q = flag;
            }
            if (((u16)(p->hB1C - 1)) <= 0x30) {
                p->state = 0x10;
            }
            break;
        case 4:
        case 5:
            p->acc += delta;
            if (flag) {
                p->state = 0;
                p->timer = 0;
                p->b1121 = 0;
                p->acc = 0;
                p->h1126 = 0;
                p->h1128 = 300;
            } else if (p->keys & 1) {
                if ((s16)p->acc > 399 || p->state == 3) {
                    p->state = 3;
                    p->timer = 0x78;
                    p->acc = 0;
                } else {
                    p->state = 2;
                }
            } else {
                p->state = 2;
            }
            break;
        case 6:
        case 7:
            if (flag) {
                p->state = 0;
                p->h1128 = 0;
                p->h112A = 0;
                p->timer = 0;
                p->acc = 0;
                p->h1126 = 0;
            } else {
                goto afec;
            }
            break;
        case 8:
        case 9:
            if (flag) {
                p->state = 0;
                p->h1126 = 0;
                p->h1128 = 0;
                p->b1121 = 0;
                p->timer = 0;
                p->acc = 420;
                p->h112A = 0;
                p->hB26 = p->h112C;
                clearFlags(&p->sub[1], 0x400008);
                clearFlags(&p->sub[0], 0x400008);
            } else {
                goto b178;
            }
            break;
        case 12:
        case 13:
            if (!(p->keys & 0x200)) {
                goto afec;
            }
            goto b178;
        case 14:
        case 15:
            if (p->keys & 0x200) {
                goto afec;
            }
        b178:
            Armor_UpdateAccumulatedBehavior(p, delta, flags);
            break;
        afec:
            Armor_UpdateContactBoost(p, delta, flags);
            break;
        case 10:
            p->acc += delta;
            if (flag) {
                p->state = 2;
                p->timer = 0x5A;
            } else if (p->keys & 1) {
                if ((s16)p->acc > 400) {
                    gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
                    v = gUnk_086141BC[gEffectRandomTableIndex];
                    if (v > 0x7FFF) {
                        p->state = 2;
                        p->acc = 0;
                    } else {
                        p->state = 1;
                        p->timer = 0x5A;
                        p->acc = 0;
                    }
                }
            } else {
                gEffectRandomTableIndex = (gEffectRandomTableIndex + 1) & 0x3FF;
                v = gUnk_086141BC[gEffectRandomTableIndex];
                if (v > 0x3FFF) {
                    p->state = 2;
                    p->timer = 0x5A;
                } else {
                    p->state = 3;
                }
            }
            break;
        case 11:
            p->acc += delta;
            if (flag) {
                p->state = 0;
                p->b1121 = 0;
                p->acc = 0;
                p->timer = 0;
                p->h1126 = 0;
            } else if (p->keys & 1) {
                p->state = 2;
            } else {
                p->state = 2;
            }
            break;
    }
}
