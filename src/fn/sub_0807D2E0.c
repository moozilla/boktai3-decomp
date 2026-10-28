// MATCHING_CONCESSION: register-pinning
#include "global.h"

#include "scene_actor_reset.h"
#include "engine.h"

// confidence=low: hit reaction for a victim actor; mode 0/1 sets a stagger state, mode 2 a knockback path, then shows damage and sparks.

#include "area_effect_link.h"
#include "runtime.h"

// confidence=low: hit reaction for a victim actor; mode 0/1 sets a stagger state, mode 2 a knockback path, then shows damage and sparks.

// confidence=low: hit reaction for a victim actor; mode 0/1 sets a stagger state, mode 2 a knockback path, then shows damage and sparks.

// confidence=low: hit reaction for a victim actor; mode 0/1 sets a stagger state, mode 2 a knockback path, then shows damage and sparks.
void SceneActorRecord_RecordPair(u32, u32);
void SceneActorAttachment_ReplaceResponseSlot(u8 *, u32, u32, u32);
void SceneActorEffects_AdvanceSteps(void *, u32);
void EffectGroup_SpawnPart(void *, s32, void *, void *, void *, s32, s32);
extern const u16 gUnk_0203B400[];

static inline u8 F0807D2E0_Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}

static inline s32 F0807D2E0_LocalPlayer(void)
{
    if (!gUnk_020004B4)
        return -1;
    return gUnk_020004B4->localPlayer;
}

static inline u8 F0807D2E0_ShouldShow(struct SceneActorRegistryResetView *actor)
{
    if (F0807D2E0_Test(gEngineFlags, 0x4800)) {
        u32 *owner = &actor->field2E0;
        register u32 mask asm("r2") = *owner;
        if (!(mask & 0x20) && mask) {
            switch (F0807D2E0_LocalPlayer()) {
                case 0:
                    if (*owner != 1)
                        return 0;
                    break;
                case 1:
                    if (*owner != 2)
                        return 0;
                    break;
                case 2:
                    if (*owner != 4)
                        return 0;
                    break;
                case 3:
                    if (*owner != 8)
                        return 0;
                    break;
            }
        }
    }
    return 1;
}

static inline void F0807D2E0_Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}

static inline void F0807D2E0_ShowDamage(struct SceneActorRegistryResetView *a)
{
    u32 zero;
    u32 buffer[2];
    s32 health = a->health;
    u32 max = *(u16 *)((u8 *)a + 0xB0);
    s32 kind = *(s16 *)((u8 *)a + 0xB2);
    s32 param = *(s16 *)((u8 *)a + 0x9A);
    SceneHud_SetTimedState(a->id, health, max, kind, 0, param);
    F0807D2E0_Fill(&zero, buffer);
    if (a->reactionSoundVariant || !(a->field2D8 & 0x20000))
        DamageNumberDisplay_Show(&a->colliderA, a->reactionSoundVariant, 0, &a->position, buffer,
                                 1);
}

static inline void F0807D2E0_SetState(struct SceneActorRegistryResetView *a, u32 state, u32 phase,
                                      u32 behavior, u32 kind, void *target)
{
    a->state2A8 = state;
    a->phase2A9 = phase;
    a->behavior2AA = behavior;
    a->counter2AC = 0;
    a->field2B1 = 1;
    a->field2B2 = 1;
    a->field2B0 = 0;
    a->field2B5 = kind;
    a->target = target;
    a->field2C8 = 0;
    a->flags114 &= ~1;
    a->word104 = 0;
}

#ifdef NONMATCHING
// fuzzy: 37.6%
// scope shape 728 (differences once low registers are set aside)
void sub_0807D2E0(void *victim, u32 mode, struct ContactPosition *position, u32 p3, u32 flip,
                  u32 p5)
{
    struct SceneActorRegistryResetView *a = victim;
    struct SceneActorRegistryControlGroup *group = &a->group;
    s32 category;
    s32 i;
    u32 pack[2];
    u32 burst[2];
    u32 hudPos[2];

    category = a->category;
    a->field2B8 = 0;

    if (a->flags2D4 & 0x2000) {
        for (i = 2; i >= 0; i--) {
            u32 idx = (gRandomIndex + 1) & 0x3FF;
            gRandomIndex = idx;
            pack[0] = (pack[0] & 0xFFFF0000) | ((gUnk_0203B400[idx] & 0xF) - 7 & 0xFFFF);
            idx = (idx + 1) & 0x3FF;
            pack[0] = (pack[0] & 0xFFFF) | (((gUnk_0203B400[idx] & 0x1F) - 16) << 16);
            gRandomIndex = idx;
            burst[0] = (pack[0] & 0xFFFF0000) | 5;
            burst[0] = (burst[0] & 0xFFFF) | (0xA0000);
            burst[1] = (burst[1] & 0xFFFF0000) | 5;
            EffectGroup_SpawnPart(&a->group, 3, group, pack, burst, 0x28, 0x28);
        }
        a->flags2D4 &= ~0x2000;
    }

    if (mode > 1) {
        if (mode == 2)
            goto mode2;
        goto done;
    }
    if ((s32)mode < 0)
        goto done;

    if (a->field2B5 != 0x15) {
        F0807D2E0_SetState(a, 3, 0, 0, 0x15, *(void **)((u8 *)group + 0x228));
        a->flags2D0 |= 0x20000;
    }
    a->flags2D0 |= 4;
    a->flags2CC &= ~0x400000;
    a->field2C2 = 0x40;
    group->flags414 = 0;
    group->field418 = 0;
    a->flags2D4 &= ~0x380;
    a->flagsEB &= ~0x18;
    if (a->flags2CC & 0x200000) {
        a->flags2D0 &= ~0x1000000;
        a->flags2D4 &= ~0x200;
        a->field2D8 = 0;
    }
    a->flags2D4 &= ~0x400;
    a->field5DA = 0xFF;
    a->position = *position;
    a->position.y += p3;
    if (a->drawableKind == 0)
        *(struct ContactPosition *)((u8 *)a->drawable + 0x1C) = a->position;
    else
        *(struct ContactPosition *)((u8 *)a->drawable + 0x40) = a->position;
    a->flags2D6 |= 0x2000;
    goto done;

mode2:
    a->flags2D0 |= 4;
    ActorShadow_Disable((u8 *)a + 0x320);
    if (a->drawableKind == 0)
        *(u32 *)a->drawable |= 1;
    else
        *(u32 *)((u8 *)a->drawable + 0x28) |= 1;
    if ((s32)flip > 0) {
        a->field2EA = 0;
        *(s16 *)((u8 *)a + 0x2E4) = *(s16 *)((u8 *)a + 0x9C);
        *(s16 *)((u8 *)&a->colliderA + 0x3E) = *(s16 *)((u8 *)a + 0x9C);
        *(s16 *)((u8 *)a + 0x9C) = 0;
        a->flags2D4 |= 8;
        a->flags2CC |= 0x400;
        a->flags2D0 |= 0xA0000;
        SceneActorAttachment_ReplaceResponseSlot((u8 *)a, 0, 0x7F, 0);
        SceneActorAttachment_ReplaceResponseSlot((u8 *)a, 1, 0x7F, 0);
        SceneActorAttachment_ReplaceResponseSlot((u8 *)a, 3, 0x7F, 0);
        SceneActorEffects_AdvanceSteps((u8 *)a, 4);
        F0807D2E0_SetState(a, 3, 0, 0, 0x11, *(void **)((u8 *)a + 0x234));
        if (F0807D2E0_ShouldShow(a))
            F0807D2E0_ShowDamage(a);
        if (!F0807D2E0_Test(gEngineFlags, 0x4800) && !(a->flags2D0 & 0x800))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, a->id);
        if (category == 4 || category == 1) {
            hudPos[0] = (hudPos[0] & 0xFFFF0000) | 0x50;
            SparkParticle_Spawn(&a->colliderA, 0x14, &a->position, 0, 2, 1, 0, 0x20);
            SparkParticle_Spawn(&a->colliderA, 0x14, &a->position, 0, 2, 1, 0, 0x40);
            SparkParticle_Spawn(&a->colliderA, 0x14, &a->position, 0, 2, 1, 0, 0x5E);
            SparkParticle_Spawn(&a->colliderA, 0x14, &a->position, 0, 2, 1, 0, 0x80);
            SparkParticle_Spawn(&a->colliderA, 0x14, &a->position, 0, 2, 1, 0, 0xC4);
            SparkParticle_Spawn(&a->colliderA, 0x14, &a->position, 0, 2, 1, 0, 0xF0);
            Sound_PlaySong(0xE5);
        }
    } else {
        s32 half = *(s16 *)((u8 *)a + 0x9C) / 2;
        s32 left;
        *(s16 *)((u8 *)a + 0x2E4) = half;
        *(s16 *)((u8 *)&a->colliderA + 0x3E) = half;
        left = *(s16 *)((u8 *)a + 0x9C) - half;
        if (left <= 0)
            left = 0;
        *(s16 *)((u8 *)a + 0x9C) = left;
        if (F0807D2E0_ShouldShow(a))
            F0807D2E0_ShowDamage(a);
    }

done:;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0807D2E0);
#endif
