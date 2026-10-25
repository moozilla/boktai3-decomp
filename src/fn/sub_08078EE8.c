#include "global.h"

extern u8 *gUnk_030042E4;
extern u32 gUnk_030053F4;
extern u32 gUnk_030053F0;
extern u8 *gUnk_02000710;
extern u32 gUnk_020004B4;
extern s32 gUnk_02000478;
extern u16 gUnk_02000534;
extern u16 gUnk_02000500;
extern u8 *gUnknown_02000580[];
extern u16 gUnk_030039F0;
extern u16 gUnk_0203B400[];
extern u32 gUnk_03005308;
extern u32 gUnk_030052F8;
extern u32 gUnk_030052FC;
extern u8 *gUnk_030052F4;

#define A_AT(T, off) (*(T *)((u8 *)arg0 + (off)))
#define B_AT(T, off) (*(T *)((u8 *)a + (off)))

void AnimData_SelectPaletteBank(void *, u32);
void SceneActorPalette_BeginTransition(u32, u32, u32, s32, u32);
void SceneActorRecord_RecordPair(u32, u32);
void SceneActorAttachment_ReplaceResponseSlot(void *, s32, s32, s32);
s32 Callback_InvokeActor(void *, void *);
void ActorShadow_Enable(void *);
void ActorShadow_Disable(void *);
void SceneActorContacts_ReleaseNode(void *);
s32 Div(s32, s32);
s32 Mod(s32, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
void SceneActorAnimation_SelectDirectionalEntry(void *, s32);
void *TerrainModifiers_FindCellOverride(s32, s32);
s32 Nav_IsPositionInMesh(void *, void *, void *);
s32 Terrain_ApplyBlocked(void *, void *, void *);
void Memory_Clear(void *, u32);
u32 TetherActor_CanStartInteraction(void);
s32 Math_Atan2(s32, s32);
void SceneActorReaction_RestoreAfterDelay(void);

/* actor kind 0: palette bank via AnimData; otherwise a raw bank pointer */
static inline void P78EE8_SetPalette(u8 *a, u32 idx)
{
    if (B_AT(u8, 8) == 0)
        AnimData_SelectPaletteBank(B_AT(u8 *, 4) + 0x2C, idx);
    else
        *(u8 **)(B_AT(u8 *, 4) + 0x68) = gUnk_030042E4 + (idx << 5);
}

/* actor exists and is dead/disabled */
static inline u32 P78EE8_IsDown(u8 *a)
{
    if (a != 0) {
        if ((B_AT(u16, 0x2D4) & 0x40) || (B_AT(u32, 0x2D0) & 0x20000) || B_AT(s16, 0x9C) <= 0)
            return 1;
    }
    return 0;
}

static inline void P78EE8_SetBodyBit(u8 *a, u32 on)
{
    u32 *w;

    if (B_AT(u8, 8) == 0)
        w = B_AT(u32 *, 4);
    else
        w = (u32 *)(B_AT(u8 *, 4) + 0x20 + 8);
    if (on)
        *w |= 1;
    else
        *w &= ~1;
}

static inline void P78EE8_ReleaseContact(u8 *a)
{
    if (B_AT(u16, 0x2D4) & 0x1000) {
        SceneActorContacts_ReleaseNode(B_AT(void *, 0x45C));
        B_AT(u16, 0x2D4) &= ~0x1000;
        B_AT(u32, 0x45C) = 0;
    }
}

/* shared reset of the knockback/stun scratch state */
static inline void P78EE8_ClearStun(u8 *a)
{
    B_AT(u16, 0x2C2) = 0x40;
    B_AT(u16, 0x414) = 0;
    B_AT(u16, 0x418) = 0;
    B_AT(u16, 0x2D4) &= ~0x380;
    B_AT(u8, 0xDC + 0xF) &= ~0x18;
    if (B_AT(u32, 0x2CC) & 0x200000) {
        B_AT(u32, 0x2D0) &= ~0x1000000;
        B_AT(u16, 0x2D4) &= ~0x200;
        B_AT(u8, 0x420 + 0xA) = 0;
        B_AT(u32, 0x420 + 0x18) = 0;
        B_AT(u16, 0x420 + 8) = 0;
    }
}

/* the dead/downed behaviour block: set action table slots */
static inline void P78EE8_SetMode(u8 *a, u8 mode, u8 sub, u32 cb, u8 b2b5)
{
    B_AT(u8, 0x2B2) = 1;
    B_AT(u8, 0x2B0) = 0;
    B_AT(u8, 0x2B5) = b2b5;
    B_AT(u32, 0x21C) = cb;
    B_AT(u16, 0x2C8) = 0;
    B_AT(u32, 0x114) &= ~1;
    B_AT(u32, 0x104) = 0;
    B_AT(u8, 0x2A8) = mode;
    B_AT(u8, 0x2A9) = sub;
    B_AT(u8, 0x2AA) = 0;
    B_AT(u32, 0x2AC) = 0;
    B_AT(u8, 0x2B1) = 1;
}

/* terrain cell index from a packed position pair */
static inline u32 P78EE8_CellIndex(s32 x, s32 y)
{
    if (x >= 0 && y >= 0 && (u32)x < gUnk_030052F8 && (u32)y < gUnk_030052FC)
        return *(u16 *)(gUnk_030052F4 + 0x24 + 2 * y) + x;
    return 0;
}

static inline s32 P78EE8_CellHeight(u32 idx, u8 *ref)
{
    void *cell;
    u8 *cp;
    s32 t, h;

    cell = TerrainModifiers_FindCellOverride(idx, 1);
    if (cell == 0)
        cp = (u8 *)(*(u32 *)(gUnk_030052F4 + 4) + (idx << 2) + 0xC);
    else
        cp = (u8 *)cell + 4;
    t = *cp >> 4;
    h = (*cp & 0xF) << 8;
    if (t == 1)
        h -= ref[4];
    else if (t == 2)
        h -= ref[0];
    return h;
}

#ifdef NONMATCHING
// fuzzy: 30.9%
// scope shape 2016 (differences once low registers are set aside)
// confidence=low: per-frame actor status update: hurt/blink timers, palette flashes, stun/knockdown state transitions, contact release, spark effect, target facing
s32 sub_08078EE8(struct SceneActorRegistryActor *arg0_)
{
    u8 *arg0 = (u8 *)arg0_;
    u8 *a = arg0;
    u16 *pTmr;
    u16 *pAA;
    s32 v;
    s32 t;
    u32 pal;
    u32 r6;
    u32 res;
    u32 cat;
    u32 go194;
    u32 sparkIdx;
    u8 ptbuf[8];
    u32 sparkSpec[2];
    s32 sparkPos[2];

    if (A_AT(u32, 0x2D0) & 0x2000)
        A_AT(u16, 0x2C2) = 0x15;
    if (A_AT(u16, 0x420 + 8) != 0)
        A_AT(u16, 0x420 + 8)--;

    /* hp countdown while not down */
    if (!P78EE8_IsDown(arg0)) {
        pAA = (u16 *)(arg0 + 0xAA);
        if ((s16)*pAA <= 0) {
            *pAA = 0;
            if (A_AT(u32, 0x2D0) & 0xC0)
                A_AT(u32, 0x2D0) |= 0x100;
        } else {
            (*pAA)--;
        }
        if (A_AT(u16, 0x5C8) != 0)
            A_AT(u16, 0x5C8)--;
    }

    /* shadow/body bit from state flags */
    if (A_AT(u16, 0x2C0) & 0x200) {
        r6 = 0;
        go194 = 1;
        if (!(A_AT(u16, 0x2D4) & 0x1000)) {
            if (!(A_AT(u16, 0x2D4) & 0xA03F) && A_AT(u16, 0x178) == 0 && A_AT(s16, 0x2EA) <= 0 &&
                A_AT(u32, 0x2EC) == 0 && A_AT(s16, 0x2F0) == 0) {
                go194 = 0;
                if (P78EE8_IsDown(arg0))
                    r6 = 1;
            }
        }
        if (((go194 || r6) && !(A_AT(u16, 0x2D4) & 0x8000)) || P78EE8_IsDown(arg0))
            A_AT(u32, 0x3D4 + 0x34) |= 1;
        else
            A_AT(u32, 0x3D4 + 0x34) &= ~1;
    }

    /* region marker at +0x5CC based on level */
    if (A_AT(u32, 0x2CC) & 0x40000) {
        v = *(s16 *)(gUnk_02000710 + 0x876);
        if (v <= 2) {
            if (A_AT(u32, 0x2CC) & 0x10000)
                A_AT(u16, 0x5CC) = 0x21;
            else
                A_AT(u16, 0x5CC) = 0x33;
        } else if (v <= 6) {
            A_AT(u16, 0x5CC) = 0x32;
        } else {
            if (A_AT(u32, 0x2CC) & 0x10000)
                A_AT(u16, 0x5CC) = 0x33;
            else
                A_AT(u16, 0x5CC) = 0x21;
        }
    }

    /* palette blink / flash */
    if (A_AT(u32, 0x2CC) & 0x8000) {
        if (A_AT(u32, 0x2CC) & 0x2000000) {
            if (A_AT(s16, 0x1EA) > 0) {
                A_AT(s16, 0x1EA)--;
            } else {
                A_AT(s16, 0x1EA) = 0;
                A_AT(u32, 0x2CC) &= ~0x2000000;
            }
        }
        pTmr = (u16 *)(arg0 + 0x1E8);
        if ((s16)*pTmr > 0) {
            (*pTmr)--;
            goto pal_done;
        }
        if ((A_AT(u32, 0x2CC) & 0x2000000) || (A_AT(u32, 0x2D0) & 0x40)) {
            if (A_AT(u32, 0x2D0) & 0x4000)
                pal = A_AT(u16, 0x1DC);
            else
                pal = A_AT(u16, 0x1E4);
        } else {
            pal = A_AT(u16, 0x1E0) + A_AT(u16, 0x1E6);
        }
        P78EE8_SetPalette(arg0, pal);
        *pTmr = 0;
        A_AT(u32, 0x2CC) &= ~0x8000;
    } else if (A_AT(u32, 0x2CC) & 0x2000000) {
        pTmr = (u16 *)(arg0 + 0x1EA);
        v = (s16)*pTmr;
        if (v > 0) {
            if (v > 0x40) {
                if (A_AT(u32, 0x2D0) & 0x4000)
                    pal = A_AT(u16, 0x1DC);
                else
                    pal = A_AT(u16, 0x1E4);
                P78EE8_SetPalette(arg0, pal);
            } else {
                if (v >= 0)
                    t = v >> 2;
                else
                    t = -((-v) >> 2);
                if (t & 1) {
                    pal = A_AT(u16, 0x1E0) + A_AT(u16, 0x1E6);
                } else if (A_AT(u32, 0x2D0) & 0x4000) {
                    pal = A_AT(u16, 0x1DC);
                } else {
                    pal = A_AT(u16, 0x1E4);
                }
                P78EE8_SetPalette(arg0, pal);
            }
            *pTmr = *pTmr - 1;
        } else {
            P78EE8_SetPalette(arg0, A_AT(u16, 0x1E0) + A_AT(u16, 0x1E6));
            *pTmr = 0;
            A_AT(u32, 0x2CC) &= ~0x2000000;
            A_AT(u32, 0x2D0) &= ~0x4000;
        }
    } else if (A_AT(u32, 0x2D0) & 0xC0) {
        if (A_AT(u32, 0x2D0) & 0x100) {
            A_AT(u32, 0x2D0) &= ~0x4140;
            if (A_AT(u32, 0x2D0) & 0x80) {
                A_AT(u32, 0x2CC) |= 0x2000000;
                A_AT(u32, 0x2D0) &= ~0x80;
                A_AT(u16, 0x1EA) = 0x40;
                goto pal_done;
            }
            P78EE8_SetPalette(arg0, A_AT(u16, 0x1E0) + A_AT(u16, 0x1E6));
            A_AT(u16, 0x1EA) = 0;
        } else {
            pTmr = (u16 *)(arg0 + 0x1EA);
            v = (s16)*pTmr;
            if (v >= 0 && v <= 0x40) {
                if (v >= 0)
                    t = v >> 2;
                else
                    t = -((-v) >> 2);
                if (t & 1) {
                    pal = A_AT(u16, 0x1E0) + A_AT(u16, 0x1E6);
                } else if (A_AT(u32, 0x2D0) & 0x4000) {
                    pal = A_AT(u16, 0x1DC);
                } else {
                    pal = A_AT(u16, 0x1E4);
                }
                P78EE8_SetPalette(arg0, pal);
                *pTmr = *pTmr + 1;
            }
        }
    }

    /* scene transition when a special mode is active */
    if (gUnk_030053F4 & 0x4800) {
        if (gUnk_020004B4 == 0)
            goto pal_done;
        res = *(s16 *)(gUnk_020004B4 + 0x18);
        if ((s32)res < 0)
            goto pal_done;
    } else {
        res = 0;
    }
    if (gUnk_030053F4 & 0x800) {
        u8 *o = gUnknown_02000580[res];
        if (o != 0) {
            u32 q = 0;
            if (*(u16 *)(o + 0x538) != 0 && *(u16 *)(o + 0x534) == 4)
                q = 1;
            res = q;
        } else {
            res = 0;
        }
    } else {
        u8 *o;
        if (gUnk_02000478 > 0) {
            res = 1;
        } else {
            o = gUnknown_02000580[res];
            if (o != 0 && (*(u32 *)(o + 0x448) & 0x10)) {
                res = 1;
            } else {
                res = 0;
                if (gUnk_02000534 != 0 && gUnk_02000500 == 4)
                    res = 1;
            }
        }
    }
    if (res) {
        if (!(A_AT(u32, 0x2CC) & 0x20000)) {
            SceneActorPalette_BeginTransition(
                A_AT(u16, 0x1DE), (u16)(A_AT(u16, 0x1E0) + A_AT(u16, 0x1E6)), 0, A_AT(u8, 0x98), 0);
            A_AT(u32, 0x2CC) |= 0x20000;
        }
    } else if (A_AT(u32, 0x2CC) & 0x20000) {
        SceneActorPalette_BeginTransition((u16)(A_AT(u16, 0x1E0) + A_AT(u16, 0x1E6)),
                                          A_AT(u16, 0x1DE), 0, A_AT(u8, 0x98), 1);
        A_AT(u32, 0x2CC) &= ~0x20000;
    }

pal_done:
    /* wake-up transition when the stun flag is set */
    if (!(A_AT(u16, 0x2D4) & 0x40) && !(A_AT(u32, 0x2D0) & 0x20000) &&
        (A_AT(u32, 0x2CC) & 0x1000)) {
        A_AT(u32, 0x2CC) &= 0xFFFEBE3F;
        A_AT(u32, 0x420 + 0x18) = 0;
        A_AT(u16, 0x420 + 0x14) = 0;
        A_AT(u16, 0x420 + 8) = 0;
        A_AT(u32, 0x2CC) &= ~0x1000;
        A_AT(u16, 0x310) = A_AT(u16, 0x5C);
        A_AT(u16, 0x314) = A_AT(u16, 0x60);
        A_AT(u16, 0x318) = A_AT(u16, 0x5C);
        A_AT(u16, 0x31C) = A_AT(u16, 0x60);
        if (A_AT(s16, 0xAA) >= A_AT(s16, 0xA6))
            A_AT(s16, 0xAA) = A_AT(s16, 0xA8);
        if (A_AT(s16, 0x9C) <= 0) {
            P78EE8_SetMode(arg0, 3, 0, A_AT(u32, 0x220), 0x11);
            if (!(gUnk_030053F4 & 0x4800) && !(A_AT(u32, 0x2D0) & 0x800))
                SceneActorRecord_RecordPair(gUnk_030039F0, (u16)A_AT(u32, 0x50));
        } else if (A_AT(u32, 0x2CC) & 0x200) {
            P78EE8_SetMode(arg0, 3, 0, A_AT(u32, 0x230), 0x12);
        } else {
            SceneActorAttachment_ReplaceResponseSlot(arg0, 3, 0x7F, A_AT(u8, 0x98));
            Callback_InvokeActor(arg0, A_AT(void *, 0x61C));
            P78EE8_SetMode(arg0, 3, 0, A_AT(u32, 0x5DC), 7);
        }
    }

    /* body bit and shadow follow the blink timer at +0x178 */
    if (!(A_AT(u16, 0x2D4) & 0x40) && !(A_AT(u32, 0x2D0) & 0x20000)) {
        if (A_AT(s16, 0x9C) <= 0) {
            if (A_AT(u16, 0x178) != 0) {
                A_AT(u16, 0x178) = 0;
                P78EE8_SetBodyBit(arg0, 0);
                if (A_AT(u8, 0x320) != 0)
                    ActorShadow_Enable(arg0 + 0x320);
            }
        } else if (A_AT(u16, 0x178) != 0 && !(A_AT(u16, 0x2D4) & 0x20)) {
            if ((A_AT(u16, 0x178) >> 2) & 1) {
                P78EE8_SetBodyBit(arg0, 0);
                if (A_AT(u8, 0x320) != 0)
                    ActorShadow_Enable(arg0 + 0x320);
            } else {
                P78EE8_SetBodyBit(arg0, 1);
                if (A_AT(u8, 0x320) != 0)
                    ActorShadow_Disable(arg0 + 0x320);
            }
            A_AT(u16, 0x178)--;
            if (A_AT(u16, 0x178) == 0) {
                P78EE8_SetBodyBit(arg0, 0);
                if (A_AT(u8, 0x320) != 0)
                    ActorShadow_Enable(arg0 + 0x320);
            }
        }
    }

    /* state-flag dispatch */
    cat = A_AT(u8, 0x98);
    if (A_AT(u16, 0x2D4) == 0) {
        A_AT(u32, 0x2CC) &= ~0x2000;
    } else if (P78EE8_IsDown(arg0)) {
        P78EE8_ReleaseContact(arg0);
        if (A_AT(u32, 0x2CC) & 0x200000) {
            A_AT(u32, 0x2D0) &= ~0x1000000;
            A_AT(u16, 0x2D4) &= ~0x200;
            A_AT(u8, 0x420 + 0xA) = 0;
            A_AT(u32, 0x420 + 0x18) = 0;
            A_AT(u16, 0x420 + 8) = 0;
        }
        A_AT(u16, 0x2D4) = 0x40;
        A_AT(u16, 0x2E6) = 0;
        A_AT(u16, 0x2D6) &= ~0x980;
    } else {
        u32 *pCC = (u32 *)(arg0 + 0x2CC);

        if (A_AT(u16, 0x2D4) & 0x2000) {
            P78EE8_ClearStun(arg0);
            A_AT(u16, 0x2D4) &= ~0x400;
            A_AT(u8, 0x5DA) = 0xFF;
            A_AT(u8, 0xDC + 0xF) &= ~8;
            A_AT(u16, 0x2D4) &= ~0x500;
            A_AT(u16, 0x2D6) |= 0x400;
            A_AT(u16, 0x2C2) = 0x40;
            A_AT(u16, 0x2D6) &= ~0x980;
        } else if (A_AT(u16, 0x2D4) & 4) {
            P78EE8_ClearStun(arg0);
            A_AT(u16, 0x2D4) &= ~0x400;
            A_AT(u8, 0x5DA) = 0xFF;
            A_AT(u8, 0xDC + 0xF) &= ~8;
            A_AT(u16, 0x2D4) &= ~0x2500;
            A_AT(u16, 0x2D6) |= 0x400;
            A_AT(u16, 0x2C2) = 0x40;
            A_AT(u16, 0x2D6) &= ~0x980;
            *pCC |= 0x2000;
        } else if (A_AT(u16, 0x2D4) & 2) {
            P78EE8_ClearStun(arg0);
            A_AT(u16, 0x2D4) &= ~0x400;
            A_AT(u8, 0x5DA) = 0xFF;
            A_AT(u8, 0xDC + 0xF) &= ~8;
            A_AT(u16, 0x2D4) &= ~0x2500;
            A_AT(u16, 0x2D6) |= 0x400;
            A_AT(u16, 0x2D6) &= ~0x980;
        } else if (A_AT(u16, 0x2D4) & 0x8000) {
            A_AT(u16, 0x2D4) &= ~0x2500;
            A_AT(u16, 0x2D6) |= 0x400;
            A_AT(u16, 0x2D6) &= ~0x980;
        } else if (A_AT(u16, 0x2D4) & 0x10) {
            *pCC &= 0xFFFEBE3F;
            A_AT(u32, 0x420 + 0x18) = 0;
            A_AT(u16, 0x420 + 0x14) = 0;
            A_AT(u16, 0x420 + 8) = 0;
            P78EE8_ClearStun(arg0);
            A_AT(u16, 0x2D4) &= ~0x400;
            A_AT(u8, 0x5DA) = 0xFF;
            A_AT(u16, 0x2D4) &= ~0x2500;
            A_AT(u16, 0xAA) = 0;
            A_AT(u16, 0x2C2) = Div(0x40, 6);
            P78EE8_ReleaseContact(arg0);
            if (*pCC & 0x200000) {
                A_AT(u32, 0x2D0) &= ~0x1000000;
                A_AT(u16, 0x2D4) &= ~0x200;
                A_AT(u8, 0x420 + 0xA) = 0;
                A_AT(u32, 0x420 + 0x18) = 0;
                A_AT(u16, 0x420 + 8) = 0;
            }
            A_AT(u16, 0x2D4) &= ~0x400;
            A_AT(u8, 0x5DA) |= 0xFF;
            A_AT(u16, 0x2D6) |= 0x400;
            A_AT(u16, 0x2D6) &= ~0x980;
        } else if (A_AT(u16, 0x2D4) & 8) {
            *pCC &= 0xFFFEBE3F;
            A_AT(u32, 0x420 + 0x18) = 0;
            A_AT(u16, 0x420 + 0x14) = 0;
            A_AT(u16, 0x420 + 8) = 0;
            if (*pCC & 0x200000) {
                A_AT(u32, 0x2D0) &= ~0x1000000;
                A_AT(u16, 0x2D4) &= ~0x200;
                A_AT(u8, 0x420 + 0xA) = 0;
                A_AT(u32, 0x420 + 0x18) = 0;
                A_AT(u16, 0x420 + 8) = 0;
            }
            A_AT(u16, 0x2D4) &= ~0x2500;
            A_AT(u16, 0x2D6) |= 0x400;
            A_AT(u16, 0x2D6) &= ~0x980;
        }

        if (A_AT(u16, 0x2D4) & 0x803E) {
            A_AT(u16, 0x2D4) &= ~0x201;
            *pCC &= ~0x4000;
            A_AT(u8, 0xDC + 0xF) &= ~0x10;
            if (!(A_AT(u16, 0x2D4) & 0x20))
                *pCC &= ~0x80;
            A_AT(u32, 0x2D0) &= ~0x100004;
            P78EE8_ReleaseContact(arg0);
            if (*pCC & 0x200000) {
                A_AT(u32, 0x2D0) &= ~0x1000000;
                A_AT(u16, 0x2D4) &= ~0x200;
                A_AT(u8, 0x420 + 0xA) = 0;
                A_AT(u32, 0x420 + 0x18) = 0;
                A_AT(u16, 0x420 + 8) = 0;
            }
            A_AT(u16, 0x2D4) &= ~0x400;
            A_AT(u8, 0x5DA) = 0xFF;
            A_AT(u16, 0x2D6) |= 0x400;
            A_AT(u16, 0x2E6) = 0;
            if (!(A_AT(u16, 0x2D4) & 0x10))
                *pCC |= 0x2000;
            else
                *pCC &= ~0x2000;
        } else {
            *pCC &= ~0x2000;
        }

        if (A_AT(u16, 0x2D4) & 0x400) {
            if (A_AT(s8, 0x5DA) < 0 || (A_AT(u8, 0x2B5) != 0x1B && A_AT(u8, 0x2B5) != 2)) {
                A_AT(u8, 0x5DA) = 0xFF;
                A_AT(u16, 0x2D4) &= ~0x400;
            }
        }
        if (A_AT(u16, 0x2D4) & 0x80) {
            if (cat == 0x15 || cat == 0x19 || cat - 5 <= 2)
                A_AT(u16, 0x2C2) = 0x20;
            else
                A_AT(u16, 0x2C2) = Div(0x40, 3);
        } else if (A_AT(u16, 0x2D4) & 0x1200) {
            if (cat == 0x15 || cat == 0x19 || cat - 5 <= 2)
                A_AT(u16, 0x2C2) = 0x20;
            else
                A_AT(u16, 0x2C2) = Div(0x40, 3);
        } else {
            A_AT(u16, 0x2D4) &= ~0x400;
            A_AT(u16, 0x414) = 0;
            A_AT(u8, 0x5DA) = 0xFF;
            A_AT(u8, 0x3D4 + 0x204) = 0;
            A_AT(u8, 0x3D4 + 0x205) = 0;
        }
    }

    /* flicker spark while the hit timer runs */
    if (A_AT(u16, 0x30C) != 0) {
        A_AT(u16, 0x30C)--;
        if (A_AT(s16, 0x5E) >= 0 && Mod(A_AT(u16, 0x30C), 4) == 0) {
            u8 baseAngle = A_AT(u8, 0x2B6);
            u32 *sp = (u32 *)sparkSpec;
            u32 rb;

            sparkPos[0] = A_AT(s32, 0x5C);
            sparkPos[1] = A_AT(s32, 0x60);
            sparkIdx = (gUnk_03005308 + 1) & 0x3FF;
            ((s16 *)sparkPos)[1] = ((s16 *)sparkPos)[1] + *(u8 *)&gUnk_0203B400[sparkIdx];
            sparkIdx = (sparkIdx + 1) & 0x3FF;
            rb = (u8)((s8)baseAngle + (gUnk_0203B400[sparkIdx] & 0x1F) - 0x10);
            sp[0] = (sp[0] & 0xFFFF0000) | 0x3C;
            sp[0] = (sp[0] & 0xFFFF) | 0x780000;
            sp[1] = (sp[1] & 0xFFFF0000) | 0x28;
            sparkIdx = (sparkIdx + 1) & 0x3FF;
            t = (gUnk_0203B400[sparkIdx] & 7) + 4;
            sparkIdx = (sparkIdx + 1) & 0x3FF;
            gUnk_03005308 = sparkIdx;
            SparkParticle_Spawn(sparkPos, 2, sparkSpec, rb, t, (gUnk_0203B400[sparkIdx] & 7) + 8, 2,
                                1);
        }
    }

    /* idle-recovery step when not down */
    if (!P78EE8_IsDown(arg0)) {
        if ((A_AT(u16, 0x2D4) & 0x80) && !(A_AT(u16, 0x2D4) & 0xA03F) && A_AT(u16, 0x178) == 0) {
            if (A_AT(u16, 0x418) != 0) {
                A_AT(u16, 0x418)--;
            } else if (cat != 2 && cat != 0x19 && !(A_AT(u16, 0x2D4) & 0x100)) {
                u8 *pt = ptbuf;
                u8 *grp;
                u8 *r5;
                u8 *src;
                s32 cx;
                s32 cy;
                u32 ci;

                P78EE8_ClearStun(arg0);
                A_AT(u16, 0x2D4) &= ~0x400;
                A_AT(u8, 0x5DA) = 0xFF;
                SceneActorAttachment_ReplaceResponseSlot(arg0, 0, 2, cat);
                SceneActorAnimation_SelectDirectionalEntry(arg0, 0);
                P78EE8_SetMode(arg0, 3, 1, (u32)SceneActorReaction_RestoreAfterDelay, 0xE);
                A_AT(u32, 0x460 + 0xC) = 0x3C;
                A_AT(u32, 0x460 + 0x10) = A_AT(u32, 0x228);
                A_AT(u8, 0x460 + 8) = 0;
                A_AT(u8, 0x460 + 9) = 0;
                A_AT(u8, 0x460 + 0xA) = 7;
                A_AT(u32, 0x2CC) |= 0x10;
                A_AT(u16, 0x2D6) &= 0xF636;

                src = A_AT(u8 *, 0x448 + 8);
                *(u16 *)pt = *(u16 *)src;
                *(u16 *)(pt + 4) = *(u16 *)(src + 2);
                ci = P78EE8_CellIndex((s8)(*(u16 *)pt >> 8), (s8)(*(u16 *)(pt + 4) >> 8));
                *(u16 *)(pt + 2) = P78EE8_CellHeight(ci, pt);

                (*(u16 **)(arg0 + 0x3D4 + 0x200))[3] = 1;
                grp = A_AT(u8 *, 0x3D4 + 0x1FC);
                r5 = arg0 + 0x5C;
                if (!Nav_IsPositionInMesh(grp, r5, pt)) {
                    A_AT(u16, 0x2D4) &= ~0x200;
                    res = 0;
                } else {
                    t = Terrain_ApplyBlocked(grp, r5, pt);
                    if (t == 0) {
                        A_AT(u16, 0x2D4) &= ~0x200;
                        res = 0;
                    } else {
                        if (t != 2)
                            Memory_Clear(arg0 + 0x64, 8);
                        res = 1;
                    }
                }
                if (!res)
                    Callback_InvokeActor(arg0, A_AT(void *, 0x3D4 + 0x224));
            }
        }
        if ((A_AT(u16, 0x2D4) & 0x1000) && !TetherActor_CanStartInteraction())
            Callback_InvokeActor(arg0, A_AT(void *, 0x618));

        if (A_AT(u16, 0x2C0) & 2) {
            A_AT(u32, 0xDC) = (u32)(gUnknown_02000580[0] + 0x24);
        } else if (A_AT(u32, 0xDC) == 0) {
            goto done;
        }
        {
            u8 *tg = A_AT(u8 *, 0xDC);
            s32 dx = *(s16 *)(tg + 0xC) - A_AT(s16, 0x5C);
            s32 dy = *(s16 *)(tg + 0x10) - A_AT(s16, 0x60);

            A_AT(u8, 0xE9) = Math_Atan2(dx, dy);
            A_AT(u32, 0xF0) = dx * dx + dy * dy;
        }
    }

done:
    A_AT(u8, 0x1CA) = A_AT(u8, 0x59);
    A_AT(u8, 0x176) = A_AT(u8, 0x59);
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08078EE8);
#endif
