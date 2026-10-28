#include "global.h"
#include "render.h"
#include "engine.h"
#include "runtime.h"
#include "ram.h"

// confidence=low: per-frame update for one scene actor's timers, palette transition and shadow state, and its spark and hit-record bookkeeping

// confidence=low: per-frame update for one scene actor's timers, palette transition and shadow state, and its spark and hit-record bookkeeping

// confidence=low: per-frame update for one scene actor's timers, palette transition and shadow state, and its spark and hit-record bookkeeping
void AnimData_SelectPaletteBank(void *, s32);
void SceneActorPalette_BeginTransition(s32, s32, s32, s32, s32);
void ActorShadow_Enable(void *);
void ActorShadow_Disable(void *);
void SparkParticle_Spawn(void *, s32, s32, s32, s32, s32);
void SceneActorRecord_RecordPair(u16, u32);
void SceneActorBehavior_UpdateColliders(void *);
void SceneActorReaction_SinkAndRemove(void);
extern u32 gLinkSession;
extern u32 gPlayers[];
extern u32 gUnk_02000478;
extern u32 gUnk_02000534;
extern u32 gUnk_02000500;
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);

#ifdef NONMATCHING
// fuzzy: 29.2%
// scope shape 990 (differences once low registers are set aside)
// confidence=low: per-frame update for one scene actor's timers, palette transition and shadow state, and its spark and hit-record bookkeeping
s32 sub_08075588(struct SceneActorRegistryActor *actor)
{
    u8 *p = (u8 *)actor;
    u32 *flags = (u32 *)(p + 0x2CC);
    u32 *flags2 = (u32 *)(p + 0x2D0);
    u16 *timer = (u16 *)(p + 0x2D4);
    s32 v;
    s32 t;
    s32 idx;
    s32 r4;
    s32 r2;
    s32 cond;

    if (*flags & 0x8000) {
        if (*flags & 0x2000000) {
            if (*(s16 *)(p + 0x1EA) > 0) {
                *(s16 *)(p + 0x1EA) -= 1;
            } else {
                *(s16 *)(p + 0x1EA) = 0;
                *flags &= 0xFDFFFFFF;
            }
        }
        if (*(s16 *)(p + 0x1E8) > 0) {
            *(s16 *)(p + 0x1E8) -= 1;
        } else if (*flags & 0x2000000 || *flags2 & 0x40) {
            if (*flags2 & 0x4000) {
                v = *(u16 *)(p + 0x1DC);
            } else {
                v = *(u16 *)(p + 0x1E4);
            }
            if (*flags2 & 0x4000) {
                v = *(u16 *)(p + 0x1DC);
            }
            if (p[8] == 0) {
                AnimData_SelectPaletteBank(*(u8 **)(p + 4) + 0x2C, 0);
            } else {
                *(u32 *)(*(u8 **)(p + 4) + 0x68) = gPaletteBankData + (v << 5);
            }
            *(u16 *)(p + 0x1E8) = 0;
            *flags &= 0xFFFF7FFF;
            goto after_timers;
        } else {
            v = *(u16 *)(p + 0x1E0) + *(u16 *)(p + 0x1E6);
            if (p[8] == 0) {
                AnimData_SelectPaletteBank(*(u8 **)(p + 4) + 0x2C, 0);
            } else {
                *(u32 *)(*(u8 **)(p + 4) + 0x68) = gPaletteBankData + (v << 5);
            }
            *(u16 *)(p + 0x1E8) = 0;
            *flags &= 0xFFFF7FFF;
        }
    } else if (*flags & 0x2000000) {
        t = *(s16 *)(p + 0x1EA);
        if (t > 0) {
            if (t > 0x40) {
                if (*flags2 & 0x4000) {
                    v = *(u16 *)(p + 0x1DC);
                } else {
                    v = *(u16 *)(p + 0x1E4);
                }
            } else {
                if (((t >> 2) & 1) != 0) {
                    v = *(u16 *)(p + 0x1E0) + *(u16 *)(p + 0x1E6);
                } else if (*flags2 & 0x4000) {
                    v = *(u16 *)(p + 0x1DC);
                } else {
                    v = *(u16 *)(p + 0x1E4);
                }
            }
            if (p[8] == 0) {
                AnimData_SelectPaletteBank(*(u8 **)(p + 4) + 0x2C, 0);
            } else {
                *(u32 *)(*(u8 **)(p + 4) + 0x68) = gPaletteBankData + (v << 5);
            }
            *(s16 *)(p + 0x1EA) -= 1;
        } else {
            v = *(u16 *)(p + 0x1E0) + *(u16 *)(p + 0x1E6);
            if (p[8] == 0) {
                AnimData_SelectPaletteBank(*(u8 **)(p + 4) + 0x2C, 0);
            } else {
                *(u32 *)(*(u8 **)(p + 4) + 0x68) = gPaletteBankData + (v << 5);
            }
            *(s16 *)(p + 0x1EA) = 0;
            *flags &= 0xFDFFFFFF;
            *flags2 &= 0xFFFFBFFF;
        }
    } else if (*flags2 & 0xC0) {
        if (*flags2 & 0x100) {
            *flags2 &= 0xFFFFBEBF;
            if (*flags2 & 0x80) {
                *flags |= 0x2000000;
                *flags2 &= 0xFFFFFF7F;
                *(u16 *)(p + 0x1EA) = 0x40;
                goto after_timers;
            }
            v = *(u16 *)(p + 0x1E0) + *(u16 *)(p + 0x1E6);
            if (p[8] == 0) {
                AnimData_SelectPaletteBank(*(u8 **)(p + 4) + 0x2C, 0);
            } else {
                *(u32 *)(*(u8 **)(p + 4) + 0x68) = gPaletteBankData + (v << 5);
            }
            *(u16 *)(p + 0x1EA) = 0;
        } else {
            t = *(s16 *)(p + 0x1EA);
            if (t > 0 && t <= 0x40) {
                if (((t >> 2) & 1) != 0) {
                    v = *(u16 *)(p + 0x1E0) + *(u16 *)(p + 0x1E6);
                } else if (*flags2 & 0x4000) {
                    v = *(u16 *)(p + 0x1DC);
                } else {
                    v = *(u16 *)(p + 0x1E4);
                }
                if (p[8] == 0) {
                    AnimData_SelectPaletteBank(*(u8 **)(p + 4) + 0x2C, 0);
                } else {
                    *(u32 *)(*(u8 **)(p + 4) + 0x68) = gPaletteBankData + (v << 5);
                }
                *(s16 *)(p + 0x1EA) += 1;
            }
        }
    }

after_timers:
    if (gEngineFlags & 0x4800) {
        if (gLinkSession == 0) {
            goto finish;
        }
        r2 = *(s16 *)(gLinkSession + 0x18);
        if (r2 < 0) {
            goto finish;
        }
    } else {
        r2 = 0;
    }
    if (gEngineFlags & 0x800) {
        u8 *e = (u8 *)gPlayers[r2];
        if (e != 0 && *(u16 *)(e + 0x538) != 0 && *(u16 *)(e + 0x534) == 4) {
            cond = 1;
        } else {
            cond = 0;
        }
    } else if (gUnk_02000478 > 0) {
        cond = 1;
    } else if (gPlayers[r2] != 0 && (*(u32 *)(gPlayers[r2] + 0x448) & 0x10) != 0) {
        cond = 1;
    } else {
        cond = (gUnk_02000534 != 0 && gUnk_02000500 == 4);
    }
    if (cond) {
        if ((*flags & 0x20000) == 0) {
            SceneActorPalette_BeginTransition(
                *(u16 *)(p + 0x1DE), *(u16 *)(p + 0x1E0) + *(u16 *)(p + 0x1E6), 0, p[0x98], 0);
            *flags |= 0x20000;
        }
    } else if ((*flags & 0x20000) != 0) {
        SceneActorPalette_BeginTransition(*(u16 *)(p + 0x1E0) + *(u16 *)(p + 0x1E6),
                                          *(u16 *)(p + 0x1DE), 0, p[0x98], 1);
        *flags &= 0xFFFDFFFF;
    }

finish:
    if (actor == 0) {
        r4 = 0;
    } else {
        r4 = (*timer & 0x40) != 0 || (*flags2 & 0x20000) != 0 || *(s16 *)(p + 0x9C) <= 0;
    }
    if (r4 == 0 && (*flags & 0x1000) != 0) {
        *flags &= 0xFFFFEFFF;
        *(u16 *)(p + 0x310) = *(u16 *)(p + 0x5C);
        *(u16 *)(p + 0x314) = *(u16 *)(p + 0x60);
        *(u16 *)(p + 0x318) = *(u16 *)(p + 0x5C);
        *(u16 *)(p + 0x31C) = *(u16 *)(p + 0x60);
        if (*(s16 *)(p + 0x9C) > 0) {
            *(u8 *)(p + 0x2B2) = 1;
            *(u8 *)(p + 0x2B0) = 0;
            *(u8 *)(p + 0x2B5) = 0x16;
            *(void **)(p + 0x21C) = SceneActorReaction_SinkAndRemove;
        } else {
            *(u8 *)(p + 0x2A8) = 3;
            *(u8 *)(p + 0x2A9) = 0;
            *(u8 *)(p + 0x2AA) = 0;
            *(u32 *)(p + 0x2AC) = 0;
            *(u8 *)(p + 0x2B1) = 1;
            *(u16 *)(p + 0x2B8) = 0;
        }
    }

    /* 0x75BB8 gate: shadow and palette-counter block */
    if ((*timer & 0x40) == 0 && (*flags2 & 0x20000) == 0) {
        if (*(s16 *)(p + 0x9C) > 0) {
            u16 c = *(u16 *)(p + 0x178);
            if (c != 0 && (*timer & 0x20) == 0) {
                if (((c >> 2) & 1) == 0) {
                    if (p[8] == 0) {
                        *(u32 *)(*(u8 **)(p + 4)) |= 1;
                    } else {
                        *(u32 *)(*(u8 **)(p + 4) + 0x28) |= 1;
                    }
                    if (p[0x320] != 0) {
                        ActorShadow_Disable(p + 0x320);
                    }
                } else {
                    if (p[8] == 0) {
                        *(u32 *)(*(u8 **)(p + 4)) &= ~2;
                    } else {
                        *(u32 *)(*(u8 **)(p + 4) + 0x28) &= ~2;
                    }
                    if (p[0x320] != 0) {
                        ActorShadow_Enable(p + 0x320);
                    }
                }
            }
        } else {
            if (*(u16 *)(p + 0x178) != 0) {
                *(u16 *)(p + 0x178) -= 1;
                if (*(u16 *)(p + 0x178) == 0) {
                    if (p[8] == 0) {
                        *(u32 *)(*(u8 **)(p + 4)) &= ~2;
                    } else {
                        *(u32 *)(*(u8 **)(p + 4) + 0x28) &= ~2;
                    }
                    if (p[0x320] != 0) {
                        ActorShadow_Enable(p + 0x320);
                    }
                }
            }
        }
    }

    if (*timer == 0) {
        *flags &= 0xFFFFDFFF;
    } else if (r4 != 0) {
        *timer = 0x40;
        *(u16 *)(p + 0x2E6) = 0;
        *(u16 *)(p + 0x2D6) &= 0xF67F;
    } else if (*timer & 0x803E) {
        *timer &= 0xFDFE;
        *flags &= 0xFFFFBFFF;
        p[0xEB] &= 0xEF;
        if ((*timer & 0x20) == 0) {
            *flags &= 0xFFFFFF7F;
        }
        *flags2 &= 0xFFEFFFFB;
        *(u16 *)(p + 0x2D6) |= 0x400;
        *(u16 *)(p + 0x2E6) = 0;
        *flags |= 0x2000;
    } else {
        *flags &= 0xFFFFDFFF;
    }

    if (*(u16 *)(p + 0x30C) != 0) {
        *(u16 *)(p + 0x30C) -= 1;
        if (*(s16 *)(p + 0x5E) >= 0 && Mod(*(u16 *)(p + 0x30C), 4) == 0) {
            u32 a18 = *(u32 *)(p + 0x5C);
            s32 pv;
            s32 n;
            s32 m;
            idx = (gRandomIndex + 1) & 0x3FF;
            a18 = (a18 & 0xFFFF) | ((u32)((s16)(a18 >> 16) + gRandomTable[idx] & 0xFF) << 16);
            idx = (idx + 1) & 0x3FF;
            pv = (s8)p[0x2B6] + ((gRandomTable[idx] & 0x1F) - 16);
            idx = (idx + 1) & 0x3FF;
            gRandomIndex = idx;
            n = (gRandomTable[idx] & 7) + 4;
            m = (gRandomTable[idx] & 0xF) + 8;
            SparkParticle_Spawn(&a18, 2, &a18, pv, n, m);
        }
    }

    if (r4 == 0) {
        cond = (*(u16 *)(p + 0x2C0) & 2) != 0;
        if (cond) {
            *(u32 *)(p + 0xDC) = (u32)(gPlayers[0] + 0x24);
        }
        if (*(u32 *)(p + 0xDC) != 0) {
            u8 *tgt = *(u8 **)(p + 0xDC);
            s32 dx = *(s16 *)(tgt + 0xC) - *(s16 *)(p + 0x5C);
            s32 dy = *(s16 *)(tgt + 0x10) - *(s16 *)(p + 0x60);
            p[0xE9] = Math_Atan2(dx, dy);
            *(u32 *)(p + 0xF0) = dx * dx + dy * dy;
        }
    }
    p[0x1CA] = p[0x59];
    p[0x176] = p[0x59];
    return 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08075588);
#endif
