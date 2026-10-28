#include "global.h"
// confidence=low: phase-dependent combat selection inferred from state and distance tests
#ifdef NONMATCHING
// fuzzy: 83.3%
struct DecisionActor {
    u8 pad0[0x51];
    u8 direction;
    u8 pad52[0x42];
    s16 vitality;
    u8 pad96[0x12];
    u8 direction4, direction8, state, padAB, action, subaction, choice, pending, queued, restart,
        finished, special;
    u8 padB4[0x10];
    u32 timer, distance;
    u8 requestedDirection;
    u8 padCD[0x1A7];
    u16 firstThreshold, secondThreshold;
    u8 pad278[0x1B0];
    u8 latch[0xC0];
    u8 transition, previousChoice, mode, index;
    u8 pad4EC[0x774];
    u32 firstSummon, secondSummon;
};
s32 SceneActors_ConsumePendingSignal(void *);
void SceneVolume_BeginPhase(void *, int);
void ProximityVolume_ClearLatch(void *);
void AnimatedHazardManager_RetractAll(void);
s32 SceneVolume_CanActivate(void *);
void SceneSteeringActor_RequestRemovalAll(void);
u8 Callback_InvokeActor(void *, void *);
extern void *gUnk_08611C6C[];
extern u32 gRandomIndex;
extern u16 gUnk_0203B400[];
static inline s32 RandomValue(void)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(u16 *)(offset + (u32)gUnk_0203B400);
}
static inline void SelectAction(struct DecisionActor *p, u32 zero, u32 choice)
{
    p->action = zero;
    p->subaction = zero;
    p->choice = choice;
    p->timer = zero;
    p->queued = 1;
}
static inline void FaceTarget(struct DecisionActor *p)
{
    p->direction = p->requestedDirection;
    {
        s32 d = (p->direction + 96) >> 6;
        u32 mask = 3;
        mask &= d;
        p->direction4 = mask;
    }
    {
        s32 d = (p->direction + 112) >> 5;
        u32 mask = 7;
        mask &= d;
        p->direction8 = mask;
    }
}
// confidence=low: selects phase-dependent attacks from the preceding choice and event index
void BossSlot2_SelectCombatAction(struct DecisionActor *p)
{
    struct DecisionActor *q = p;
    u8 *event = &p->index;
    if (SceneActors_ConsumePendingSignal(p))
        SceneVolume_BeginPhase(p, 4);
    if (!(*(u32 *)&p->transition & 0xFF0000FF))
        goto callback;
    switch (p->mode) {
        case 0:
            if (p->vitality < p->firstThreshold) {
                if (p->firstSummon) {
                    p->special = 1;
                    p->action = 2;
                    p->subaction = 0;
                    p->choice = 0;
                    p->timer = 0;
                    p->queued = 1;
                    p->state = 1;
                } else {
                    p->mode = 1;
                    SelectAction(p, 0, 8);
                }
            } else {
                switch (p->previousChoice) {
                    case 0: {
                        u32 index = *(event = &q->index);
                        if (index == 4) {
                            SelectAction(p, 0, 11);
                        } else if (index == 1) {
                            if (p->distance <= 0x23FFF) {
                                SelectAction(p, 0, 2);
                            } else {
                                SelectAction(p, 0, 1);
                            }
                        } else if (index == 2) {
                            FaceTarget(p);
                            SelectAction(p, 0, 6);
                        } else if (index == 3) {
                            SelectAction(p, 0, 5);
                        } else {
                            SelectAction(p, 0, 3);
                        }
                    } break;
                    case 1: {
                        u32 index = *(event = &q->index);
                        if (index == 4) {
                            SelectAction(p, 0, 11);
                        } else if (index != 0) {
                            SelectAction(p, 0, 2);
                        } else {
                            SelectAction(p, index, 4);
                        }
                    } break;
                    case 2: {
                        s32 result = SceneVolume_CanActivate(q);
                        if (result) {
                            SelectAction(p, 0, 12);
                        } else {
                            u32 index = *(event = &q->index);
                            if (index == 0) {
                                SelectAction(p, result, 4);
                            } else if (index == 2) {
                                SelectAction(p, result, 14);
                            }
                        }
                    } break;
                    case 3: {
                        u32 index = *(event = &q->index);
                        if (index == 4) {
                            SelectAction(p, 0, 11);
                        } else if (index == 1) {
                            SelectAction(p, 0, 6);
                        } else if (index == 3) {
                            SelectAction(p, 0, 5);
                        } else if (index == 2) {
                            FaceTarget(p);
                            SelectAction(p, 0, 6);
                        } else {
                            SelectAction(p, 0, 3);
                        }
                    } break;
                    case 4: {
                        u32 index = *(event = &q->index);
                        if (index == 4) {
                            SelectAction(p, 0, 11);
                        } else if ((u8)(index - 1) <= 1) {
                            if (p->distance <= 0x23FFF) {
                                SelectAction(p, 0, 2);
                            } else {
                                SelectAction(p, 0, 1);
                            }
                        } else if (index == 3) {
                            SelectAction(p, 0, 5);
                        } else {
                            SelectAction(p, 0, 3);
                        }
                    } break;
                    case 5: {
                        u32 index = *(event = &q->index);
                        if (index == 4) {
                            SelectAction(p, 0, 11);
                        } else if (index == 1) {
                            SelectAction(p, 0, 6);
                        } else if (index == 3) {
                            SelectAction(p, 0, 5);
                        } else {
                            SelectAction(p, 0, 7);
                        }
                    } break;
                    case 6: {
                        u32 index = *(event = &q->index);
                        if (index == 4) {
                            SelectAction(p, 0, 11);
                        } else {
                            if (p->distance <= 0x23FFF) {
                                SelectAction(p, 0, 2);
                            } else {
                                SelectAction(p, 0, 1);
                            }
                        }
                    } break;
                    case 7: {
                        u32 index = *(event = &q->index);
                        if (index == 4) {
                            SelectAction(p, 0, 11);
                        } else if (index == 1) {
                            SelectAction(p, 0, 6);
                        } else if (index == 3) {
                            SelectAction(p, 0, 5);
                        } else if (index == 2) {
                            FaceTarget(p);
                            SelectAction(p, 0, 6);
                        } else {
                            SelectAction(p, 0, 4);
                        }
                    } break;
                    case 11: {
                        if (p->distance <= 0x23FFF) {
                            SelectAction(p, 0, 2);
                        } else {
                            SelectAction(p, 0, 1);
                        }
                    } break;
                    case 12: {
                        SelectAction(p, 0, 13);
                    } break;
                    case 13: {
                        u32 index = *(event = &q->index);
                        if (index == 0) {
                            SelectAction(p, index, 14);
                        }
                    } break;
                    case 14: {
                        u32 index = *(event = &q->index);
                        if (index == 2) {
                            if (p->distance <= 0x23FFF) {
                                SelectAction(p, 0, 2);
                            } else {
                                SelectAction(p, 0, 1);
                            }
                        } else {
                            SelectAction(p, 0, 3);
                        }
                    } break;
                }
            }
            break;
        case 1:
            if (p->vitality < p->secondThreshold) {
                ProximityVolume_ClearLatch(p->latch);
                AnimatedHazardManager_RetractAll();
                if (p->secondSummon) {
                    p->special = 1;
                    p->action = 2;
                    p->subaction = 0;
                    p->choice = 0;
                    p->timer = 0;
                    p->queued = 1;
                    p->state = 1;
                } else {
                    p->mode = 2;
                    SelectAction(p, 0, 10);
                }
            } else {
                switch (p->previousChoice) {
                    case 0:
                    case 1:
                    case 3:
                    case 6:
                    case 7:
                        SelectAction(p, 0, 8);
                        break;
                    case 2: {
                        s32 result = SceneVolume_CanActivate(q);
                        if (result) {
                            SelectAction(p, 0, 12);
                        } else {
                            u32 index = *(event = &q->index);
                            if (index == 0) {
                                SelectAction(p, result, 8);
                            } else if (index == 2) {
                                SelectAction(p, result, 14);
                            }
                        }
                    } break;
                    case 8: {
                        u32 index = *(event = &q->index);
                        if (index == 2) {
                            if (p->distance <= 0x23FFF) {
                                SelectAction(p, 0, 2);
                            } else {
                                SelectAction(p, 0, 1);
                            }
                            ProximityVolume_ClearLatch(q->latch);
                            AnimatedHazardManager_RetractAll();
                        } else {
                            SelectAction(p, 0, 9);
                        }
                    } break;
                    case 9: {
                        u32 index = *(event = &q->index);
                        if (index == 2) {
                            SelectAction(p, 0, 13);
                        } else {
                            SelectAction(p, 0, 8);
                        }
                        ProximityVolume_ClearLatch(q->latch);
                        AnimatedHazardManager_RetractAll();
                    } break;
                    case 11: {
                        if (p->distance <= 0x23FFF) {
                            SelectAction(p, 0, 2);
                        } else {
                            SelectAction(p, 0, 1);
                        }
                    } break;
                    case 12: {
                        SelectAction(p, 0, 13);
                    } break;
                    case 13: {
                        u32 index = *(event = &q->index);
                        if (index == 0) {
                            SelectAction(p, index, 8);
                        }
                    } break;
                    case 14: {
                        u32 index = *(event = &q->index);
                        if (index == 2) {
                            if (p->distance <= 0x23FFF) {
                                SelectAction(p, 0, 2);
                            } else {
                                SelectAction(p, 0, 1);
                            }
                        } else {
                            SelectAction(p, 0, 3);
                        }
                    } break;
                }
            }
            break;
        case 2:
            switch (p->previousChoice) {
                case 0: {
                    u32 index = *(event = &q->index);
                    if (index == 1) {
                        if (p->distance <= 0x23FFF) {
                            SelectAction(p, 0, 2);
                        } else {
                            SelectAction(p, 0, 1);
                        }
                    } else if (index == 2) {
                        FaceTarget(p);
                        SelectAction(p, 0, 6);
                    } else if (index == 3) {
                        SelectAction(p, 0, 5);
                    } else {
                        SelectAction(p, 0, 3);
                    }
                } break;
                case 1: {
                    u32 index = *(event = &q->index);
                    if (index == 4) {
                        SelectAction(p, 0, 11);
                    } else if (index != 0) {
                        SelectAction(p, 0, 2);
                    } else {
                        SelectAction(p, index, 4);
                    }
                } break;
                case 2: {
                    s32 result = SceneVolume_CanActivate(q);
                    if (result) {
                        SelectAction(p, 0, 12);
                    } else {
                        u32 index = *(event = &q->index);
                        if (index == 0) {
                            SelectAction(p, result, 4);
                        } else if (index == 2) {
                            SelectAction(p, result, 14);
                        }
                    }
                } break;
                case 3: {
                    u32 index = *(event = &q->index);
                    if (index == 4) {
                        SelectAction(p, 0, 11);
                    } else if (index == 1) {
                        SelectAction(p, 0, 6);
                    } else if (index == 3) {
                        SelectAction(p, 0, 5);
                    } else if (index == 2) {
                        FaceTarget(p);
                        SelectAction(p, 0, 6);
                    } else {
                        u32 random = (RandomValue() >> 8) & 7;
                        if (random == 0) {
                            SelectAction(p, random, 10);
                        } else {
                            SelectAction(p, 0, 3);
                        }
                    }
                } break;
                case 4: {
                    u32 index = *(event = &q->index);
                    if (index == 4) {
                        SelectAction(p, 0, 11);
                    } else if ((u8)(index - 1) <= 1) {
                        if (p->distance <= 0x23FFF) {
                            SelectAction(p, 0, 2);
                        } else {
                            SelectAction(p, 0, 1);
                        }
                    } else if (index == 3) {
                        SelectAction(p, 0, 5);
                    } else {
                        u32 random = (RandomValue() >> 8) & 1;
                        if (random == 0) {
                            SelectAction(p, random, 10);
                        } else {
                            SelectAction(p, 0, 3);
                        }
                    }
                } break;
                case 5: {
                    u32 index = *(event = &q->index);
                    if (index == 4) {
                        SelectAction(p, 0, 11);
                    } else if (index == 1) {
                        SelectAction(p, 0, 6);
                    } else if (index == 3) {
                        SelectAction(p, 0, 5);
                    } else {
                        SelectAction(p, 0, 7);
                    }
                } break;
                case 6: {
                    u32 index = *(event = &q->index);
                    if (index == 4) {
                        SelectAction(p, 0, 11);
                    } else {
                        if (p->distance <= 0x23FFF) {
                            SelectAction(p, 0, 2);
                        } else {
                            SelectAction(p, 0, 1);
                        }
                    }
                } break;
                case 7: {
                    u32 index = *(event = &q->index);
                    if (index == 4) {
                        SelectAction(p, 0, 11);
                    } else if (index == 1) {
                        SelectAction(p, 0, 6);
                    } else if (index == 3) {
                        SelectAction(p, 0, 5);
                    } else if (index == 2) {
                        FaceTarget(p);
                        SelectAction(p, 0, 6);
                    } else {
                        SelectAction(p, 0, 4);
                    }
                } break;
                case 10: {
                    if (SceneVolume_CanActivate(q)) {
                        SceneSteeringActor_RequestRemovalAll();
                        SelectAction(p, 0, 12);
                    } else {
                        u32 index = *(event = &q->index);
                        if (index == 0) {
                            SelectAction(p, index, 3);
                        }
                    }
                } break;
                case 11: {
                    if (p->distance <= 0x23FFF) {
                        SelectAction(p, 0, 2);
                    } else {
                        SelectAction(p, 0, 1);
                    }
                } break;
                case 12: {
                    SelectAction(p, 0, 13);
                } break;
                case 13: {
                    u32 index = *(event = &q->index);
                    if (index == 0) {
                        SelectAction(p, index, 0);
                    }
                } break;
                case 14: {
                    u32 index = *(event = &q->index);
                    if (index == 2) {
                        if (p->distance <= 0x23FFF) {
                            SelectAction(p, 0, 2);
                        } else {
                            SelectAction(p, 0, 1);
                        }
                    } else {
                        u32 random = (RandomValue() >> 8) & 1;
                        if (random == 0) {
                            SelectAction(p, random, 10);
                        } else {
                            SelectAction(p, 0, 3);
                        }
                    }
                } break;
            }
            break;
    }
    *event = 0;
    q->transition = 0;
    if (*(u16 *)&p->action == 0) {
    callback:
        Callback_InvokeActor(p, gUnk_08611C6C[p->choice]);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", BossSlot2_SelectCombatAction);
#endif
