#include "global.h"
#include "scene_actor_reaction.h"
#include "area_effect_link.h"
#include "render.h"
#include "runtime.h"
/* Clears hit timers; at zero health hands control to the defeat target and records the defeat. */
void SceneActorRecord_RecordPair(u16, u16);
static inline u8 Test(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
static inline u32 Flag2D0(struct SceneActorRegistryResetView *actor, u32 maskInput)
{
    u32 mask = maskInput;
    return actor->flags2D0 & mask;
}
static inline void Reaction_SetControl(struct SceneActorRegistryResetView *actor, u32 kind,
                                       void *control)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void Hide(struct SceneActorRegistryResetView *a)
{
    if (!a->drawableKind) {
        u32 *flags = a->drawable;
        u32 mask = ~1;
        *flags &= mask;
    } else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)a->drawable + 0x20);
        u32 mask = ~1;
        tile->flags &= mask;
    }
}

void AnimData_SelectPaletteBank(void *, s32);
void SceneHud_SetTimedState(u32, s32, s32, s32, s32, s32);
void CpuSet(const void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneActorMovement_ProjectProbe(void *, u32, u32);
void Sound_PlaySong(u32);
static inline s32 LocalPlayer(void)
{
    if (!gLinkSession)
        return -1;
    return gLinkSession->localPlayer;
}
static inline u16 OwnershipFlagBits(u32 value, u32 mask)
{
    return value & mask;
}
static inline u8 ShouldShowDamage(struct SceneActorRegistryResetView *actor)
{
    if (Test(gEngineFlags, 0x4800)) {
        u32 *owner = &actor->field2E0;
        u32 mask = *owner;
        if (!OwnershipFlagBits(mask, 0x20) && mask) {
            switch (LocalPlayer()) {
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

static inline void Fill(u32 *zero, void *dst)
{
    *zero = 0;
    CpuSet(zero, dst, 0x05000002);
}
void SceneActorReaction_ApplyCategoryDamage(struct SceneActorRegistryResetView *actor)
{
    u8 *collider = (u8 *)&actor->colliderA;
    {
        u32 damage = actor->reactionSoundVariant;
        actor->health -= damage;
    }
    if (!(actor->field2D8 & 0x20000)) {
        u16 bank = *(u16 *)((u8 *)actor + 0x1E2);
        if (actor->drawableKind == 0)
            AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, bank);
        else
            *(u8 **)((u8 *)actor->drawable + 0x68) = gPaletteBankData + bank * 32;
        {
            u32 eight = 8;
            actor->field1E8 = eight;
        }
        {
            u32 bit = 0x8000;
            actor->flags2CC |= bit;
        }
        if (ShouldShowDamage(actor)) {
            u32 zero;
            u32 buffer[2];
            s32 health = actor->health;
            u32 max = *(u16 *)((u8 *)actor + 0xB0);
            s32 kind = *(s16 *)((u8 *)actor + 0xB2);
            s32 param = *(s16 *)((u8 *)actor + 0x9A);
            SceneHud_SetTimedState(actor->body.id, health, max, kind, 0, param);
            Fill(&zero, buffer);
            if (actor->reactionSoundVariant || !(actor->field2D8 & 0x20000))
                DamageNumberDisplay_Show(&actor->colliderA, actor->reactionSoundVariant, 0,
                                         &actor->body.position, buffer, 1);
        }
    }
    SceneActorMovement_ProjectProbe(actor, 0, 0);
    {
        u16 *flags = &actor->flags2D4;
        u32 mask = ~0x2000;
        u16 value = mask & *flags;
        u32 zero = 0;
        *flags = value;
        actor->field2B8 = zero;
    }
    SceneActorTerrain_ClearCollisionTimers(actor);
    {
        u16 *flags = &actor->flags2D6;
        u32 mask = ~0x980;
        *flags = mask & *flags;
    }
    if (actor->health <= 0) {
        *(u16 *)(collider + 0x44) = 0;
        Hide(actor);
        {
            void *control = *(void **)((u8 *)actor + 0x234);
            Reaction_SetControl(actor, 17, control);
        }
        {
            u32 state = 3;
            actor->state2A8 = state;
            actor->phase2A9 = 0;
            actor->behavior2AA = 0;
            actor->counter2AC = 0;
            actor->field2B1 = 1;
        }
        actor->field2E0 = 32;
        if (!Test(gEngineFlags, 0x4800) && !Flag2D0(actor, 0x800))
            SceneActorRecord_RecordPair(gScriptEntryProcedure, actor->body.id);
    } else {
        Sound_PlaySong(0x13E);
    }
}
