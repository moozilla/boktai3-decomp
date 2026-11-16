#ifndef GUARD_SCENE_ACTOR_STATUS_H
#define GUARD_SCENE_ACTOR_STATUS_H
#include "scene_actor_chase_reaction.h"
#include "player.h"
#include "global.h"
#include "render.h"
#include "ram.h"
#include "runtime.h"
#include "engine_globals.h"
#include "scene_actor_contact_list.h"

extern u32 gLinkSession;
extern u8 *gUnknown_02000580[];
void SceneActorPalette_BeginTransition(u32, u32, u32, s32, u32);
void ActorShadow_Enable(void *);
void ActorShadow_Disable(void *);
s32 Mod(s32, s32);
void SparkParticle_Spawn(void *, s32, void *, s32, s32, s32, s32, s32);
s32 Math_Atan2(s32, s32);

/* Signed countdown interpretation of the canonical raw halfword. */
struct SceneActorPaletteTimer {
    s16 remaining;
} __attribute__((packed, aligned(2)));

/* Link state and solar level sampled by the scene actor palette routines. */
struct SceneEnvironmentSnapshot {
    u32 linkMode;
    s32 solarLevel;
};

static inline void ActorStatus_SetWord(u32 *field, u32 bits)
{
    *field |= bits;
}

static inline u32 ActorStatus_BehaviorFlags(void *object, u32 mask)
{
    struct ChaseActor *actor = object;
    return actor->base.behaviorFlags & mask;
}

static inline u32 ActorStatus_StatusFlags(void *object, u32 mask)
{
    struct ChaseActor *actor = object;
    return actor->base.statusFlags & mask;
}

static inline u32 ActorStatus_BitsPresent(u32 bits)
{
    if (bits)
        return 1;
    return 0;
}

static inline u32 ActorStatus_HasStatusFlags(void *object, u32 mask)
{
    return ActorStatus_BitsPresent(ActorStatus_StatusFlags(object, mask));
}

static inline u16 ActorStatus_MotionFlags(void *object, u16 mask)
{
    struct ChaseActor *actor = object;
    return SceneActorChase_FlagComponentBits((void *)&actor->base.motionFlags, mask);
}

static inline u8 ActorStatus_BodyFlags(struct ChaseActor *actor, u32 mask)
{
    if (actor->base.placementFlags & mask)
        return 1;
    return 0;
}

static inline void ActorStatus_ClearTrackingFlags(struct SceneActorReactionMotion *tracking,
                                                  u32 bits)
{
    u32 mask = ~bits;
    tracking->flags &= mask;
}

static inline s16 ActorStatus_FlashRemaining(struct ChaseActor *actor)
{
    return (*(s16 *)&actor->base.flashTimer);
}

static inline s16 ActorStatus_BlinkRemaining(struct ChaseActor *actor)
{
    return ((struct SceneActorPaletteTimer *)&actor->base.paletteTimer)->remaining;
}

static inline void ActorStatus_StoreByte(u8 *field, u8 value)
{
    *field = value;
}

static inline void ActorStatus_SetPalette(struct ChaseActor *actor, u32 idx)
{
    if (actor->base.drawableKind == 0)
        AnimData_SelectPaletteBank(
            ((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->animation, idx);
    else
        ((struct SceneActorRegistryTileDrawable *)actor->base.drawable)->palette =
            gPaletteBankData + (idx << 5);
}

static inline u8 ActorStatus_IsDown(void *object)
{
    struct ChaseActor *actor = object;
    if (actor == 0)
        return 0;
    if (ActorStatus_MotionFlags((void *)actor, 0x40) ||
        ActorStatus_HasStatusFlags(actor, 0x20000) || actor->base.health <= 0)
        return 1;
    return 0;
}

static inline void ActorStatus_SetDrawableFlags(struct ChaseActor *actor, u32 bits)
{
    if (actor->base.drawableKind == 0)
        ((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->flags |= bits;
    else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->base.drawable + 0x20);
        tile->flags |= bits;
    }
}

static inline void ActorStatus_ClearTileFlags(struct SceneActorAnimationTile *tile, u32 bits)
{
    u32 keep = ~bits;
    tile->flags &= keep;
}

static inline void ActorStatus_ClearDrawableFlags(struct ChaseActor *actor, u32 bits)
{
    if (actor->base.drawableKind == 0)
        SceneActorChase_ClearWordFlags(
            &((struct SceneActorRegistrySequenceDrawable *)actor->base.drawable)->flags, bits);
    else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->base.drawable + 0x20);
        ActorStatus_ClearTileFlags(tile, bits);
    }
}

static inline void ActorStatus_SetBodyBit(struct ChaseActor *actor, u32 on)
{
    if (on)
        ActorStatus_SetDrawableFlags(actor, 1);
    else
        ActorStatus_ClearDrawableFlags(actor, 1);
}

static inline void ActorStatus_ClearMotionFlags(struct ChaseActor *actor, u32 bits)
{
    actor->base.motionFlags &= ~bits;
}

static inline void ActorStatus_ReleaseContact(struct ChaseActor *actor)
{
    u32 flag = 0x1000;
    if (actor->base.motionFlags & flag) {
        SceneActorContacts_ReleaseNode(actor->group.contactNode);
        actor->base.motionFlags &= ~flag;
        actor->group.contactNode = 0;
    }
}

static inline void ActorStatus_ClearInteraction(struct ChaseActor *actor,
                                                struct SceneActorRegistryControl *control)
{
    SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 0x1000000);
    ActorStatus_ClearMotionFlags(actor, 0x200);
    control->mode = 0;
    control->counter = 0;
    control->timer = 0;
}

static inline struct SceneEnvironmentSnapshot SceneEnvironment_Read(void)
{
    struct SceneEnvironmentSnapshot result;
    result.linkMode = SceneActorChase_Test(gEngineFlags, 0x4800);
    result.solarLevel = gGameState->unk876;
    return result;
}

static inline u32 SceneEnvironment_LinkMode(void)
{
    struct SceneEnvironmentSnapshot result = SceneEnvironment_Read();
    return result.linkMode;
}

static inline u32 ActorStatus_DefaultPalette(struct ChaseActor *actor)
{
    u16 *base = &actor->base.basePaletteBank;
    u16 *offset = &actor->base.paletteOffset;
    return *base + *offset;
}

static inline u8 ActorStatus_PassagePaletteActive(void)
{
    u16 passage = gPlayerPassageState;
    u32 active = 0;
    if (passage != 0 && gPlayerInteractionMode == 4)
        active = 1;
    return active;
}

static inline u8 ActorStatus_LinkPaletteActive(struct Player *object)
{
    if (object != 0) {
        u16 state = object->active;
        u32 active = 0;
        if (state != 0 && object->sceneMode == 4)
            active = 1;
        return active;
    }
    return 0;
}

static inline u32 ActorStatus_PalettePlayerFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}

static inline u32 ActorStatus_EnvironmentPaletteActive(s32 player)
{
    struct Player *object;
    if (gEngineFlags & 0x800) {
        return ActorStatus_LinkPaletteActive((struct Player *)gUnknown_02000580[player]);
    }
    if (gWeatherCollisionTimer > 0)
        return 1;
    object = (struct Player *)gUnknown_02000580[player];
    if (object != 0 && ActorStatus_PalettePlayerFlags(&object->actionFlags, 0x10))
        return 1;
    return ActorStatus_PassagePaletteActive();
}

static inline void ActorStatus_SetBlinkTimer(struct ChaseActor *actor, s16 timer)
{
    ((struct SceneActorPaletteTimer *)&actor->base.paletteTimer)->remaining = timer;
}

static inline void ActorStatus_Palette(struct ChaseActor *actor)
{
    s16 *pTmr;

    u32 res;
    /* palette blink / flash */
    if (ActorStatus_BehaviorFlags(actor, 0x8000)) {
        s32 flashRemaining;
        s16 *flashTimer;
        if (ActorStatus_BehaviorFlags(actor, 0x2000000)) {
            s32 blinkRemaining = ActorStatus_BlinkRemaining(actor);
            if (blinkRemaining > 0) {
                ((struct SceneActorPaletteTimer *)&actor->base.paletteTimer)->remaining =
                    blinkRemaining - 1;
            } else {
                ActorStatus_SetBlinkTimer(actor, 0);
                SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x2000000);
            }
        }
        flashRemaining = ActorStatus_FlashRemaining(actor);
        flashTimer = &(*(s16 *)&actor->base.flashTimer);
        if (flashRemaining > 0) {
            *flashTimer = flashRemaining - 1;
            return;
        }
        if ((ActorStatus_BehaviorFlags(actor, 0x2000000)) ||
            (ActorStatus_HasStatusFlags(actor, 0x40))) {
            if (ActorStatus_HasStatusFlags(actor, 0x4000))
                ActorStatus_SetPalette(actor, actor->base.alternatePalette);
            else
                ActorStatus_SetPalette(actor, actor->base.alternateOffset);
        } else {
            ActorStatus_SetPalette(actor, ActorStatus_DefaultPalette(actor));
        }
        *flashTimer = 0;
        SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x8000);
        return;
    } else if (ActorStatus_BehaviorFlags(actor, 0x2000000)) {
        s32 blinkRemaining;
        s32 phase;
        blinkRemaining = ActorStatus_BlinkRemaining(actor);
        pTmr = &((struct SceneActorPaletteTimer *)&actor->base.paletteTimer)->remaining;
        if (blinkRemaining > 0) {
            if (blinkRemaining <= 0x40) {
                if (blinkRemaining >= 0)
                    phase = blinkRemaining >> 2;
                else
                    phase = -((-blinkRemaining) >> 2);
                if (phase & 1) {
                    ActorStatus_SetPalette(actor, ActorStatus_DefaultPalette(actor));
                } else if (ActorStatus_HasStatusFlags(actor, 0x4000)) {
                    ActorStatus_SetPalette(actor, actor->base.alternatePalette);
                } else {
                    ActorStatus_SetPalette(actor, actor->base.alternateOffset);
                }
            } else {
                if (ActorStatus_HasStatusFlags(actor, 0x4000))
                    ActorStatus_SetPalette(actor, actor->base.alternatePalette);
                else
                    ActorStatus_SetPalette(actor, actor->base.alternateOffset);
            }
            *pTmr = *pTmr - 1;
        } else {
            ActorStatus_SetPalette(actor, ActorStatus_DefaultPalette(actor));
            *pTmr = 0;
            SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x2000000);
            SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 0x4000);
        }
    } else if (ActorStatus_HasStatusFlags(actor, 0xC0)) {
        if (ActorStatus_HasStatusFlags(actor, 0x100)) {
            SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 0x4140);
            if (ActorStatus_HasStatusFlags(actor, 0x80)) {
                ActorStatus_SetWord(&actor->base.behaviorFlags, 0x2000000);
                SceneActorChase_ClearWordFlags(&actor->base.statusFlags, 0x80);
                ActorStatus_SetBlinkTimer(actor, 0x40);
                return;
            }
            ActorStatus_SetPalette(actor, ActorStatus_DefaultPalette(actor));
            ActorStatus_SetBlinkTimer(actor, 0);
        } else {
            s32 recoveryRemaining;
            s32 phase;
            recoveryRemaining = ActorStatus_BlinkRemaining(actor);
            pTmr = &((struct SceneActorPaletteTimer *)&actor->base.paletteTimer)->remaining;
            if (recoveryRemaining >= 0) {
                if (recoveryRemaining <= 0x40) {
                    if (recoveryRemaining >= 0)
                        phase = recoveryRemaining >> 2;
                    else
                        phase = -((-recoveryRemaining) >> 2);
                    if (phase & 1) {
                        ActorStatus_SetPalette(actor, ActorStatus_DefaultPalette(actor));
                    } else if (ActorStatus_HasStatusFlags(actor, 0x4000)) {
                        ActorStatus_SetPalette(actor, actor->base.alternatePalette);
                    } else {
                        ActorStatus_SetPalette(actor, actor->base.alternateOffset);
                    }
                    *pTmr = *pTmr + 1;
                }
            }
        }
    }

    /* scene transition when a special mode is active */
    if (SceneEnvironment_LinkMode()) {
        if (gLinkSession == 0)
            return;
        res = *(s16 *)(gLinkSession + 0x18);
        if ((s32)res >= 0)
            goto valid_player;
        return;
    }
    res = 0;
valid_player:
    if (ActorStatus_EnvironmentPaletteActive(res)) {
        if (!(ActorStatus_BehaviorFlags(actor, 0x20000))) {
            u32 current = actor->base.paletteBank;
            u32 restored = ActorStatus_DefaultPalette(actor);
            SceneActorPalette_BeginTransition(current, restored, 0, actor->base.category, 0);
            ActorStatus_SetWord(&actor->base.behaviorFlags, 0x20000);
        }
    } else if (ActorStatus_BehaviorFlags(actor, 0x20000)) {
        SceneActorPalette_BeginTransition((ActorStatus_DefaultPalette(actor)),
                                          actor->base.paletteBank, 0, actor->base.category, 1);
        SceneActorChase_ClearWordFlags(&actor->base.behaviorFlags, 0x20000);
    }
}

static inline u16 ActorStatus_TickResponse(struct ContactCollider *response)
{
    return --response->responseValue;
}

static inline void ActorStatus_Blink(struct ChaseActor *actor)
{

    /* body bit and shadow follow the blink timer at +0x178 */
    if (!(ActorStatus_MotionFlags(actor, 0x40)) && !(ActorStatus_StatusFlags(actor, 0x20000))) {
        struct ContactCollider *response = &actor->base.colliderA;
        if (actor->base.health <= 0) {
            if (actor->base.colliderA.responseValue != 0) {
                actor->base.colliderA.responseValue = 0;
                ActorStatus_SetBodyBit(actor, 0);
                if (actor->base.shadow[0] != 0)
                    ActorShadow_Enable((u8 *)actor + 0x320);
            }
        } else if (actor->base.colliderA.responseValue != 0 &&
                   !(ActorStatus_MotionFlags(actor, 0x20))) {
            if ((actor->base.colliderA.responseValue >> 2) & 1) {
                ActorStatus_SetBodyBit(actor, 0);
                if (actor->base.shadow[0] != 0)
                    ActorShadow_Enable((u8 *)actor + 0x320);
            } else {
                ActorStatus_SetBodyBit(actor, 1);
                if (actor->base.shadow[0] != 0)
                    ActorShadow_Disable((u8 *)actor + 0x320);
            }
            if (ActorStatus_TickResponse(response) == 0) {
                ActorStatus_SetBodyBit(actor, 0);
                if (actor->base.shadow[0] != 0)
                    ActorShadow_Enable((u8 *)actor + 0x320);
            }
        }
    }
}

static inline void ActorStatus_SetPendingBits(struct ChaseActor *actor, u16 bits)
{
    actor->base.reactionFlags |= bits;
}

static inline u32 ActorStatus_RandomSample(const u16 *samples)
{
    u32 offset;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    offset = gRandomIndex * 2;
    return *(const u16 *)(offset + (u32)samples);
}

static inline u8 ActorStatus_ParticleHeading(s8 heading, s32 jitter)
{
    return heading + jitter;
}

static inline void ActorStatus_Particles(struct ChaseActor *actor)
{
    u32 lifetime;
    s32 t;
    struct ContactPackedVector sparkSpec;
    union {
        struct ContactPosition position;
        struct ContactPackedVector packed;
    } sparkPos;
    /* flicker spark while the hit timer runs */
    if ((*(u16 *)&actor->base.particleTimer) != 0) {
        (*(u16 *)&actor->base.particleTimer)--;
        if (actor->base.body.position.y >= 0 && Mod((*(u16 *)&actor->base.particleTimer), 4) == 0) {
            u8 baseAngle = actor->base.reactionDirection;
            u32 rb;

            sparkPos.position = actor->base.body.position;
            sparkPos.packed.y += (ActorStatus_RandomSample(gRandomTable) & 255);
            rb = ActorStatus_ParticleHeading(
                baseAngle, (s32)(ActorStatus_RandomSample(gRandomTable) & 31) - 16);
            sparkSpec.x = 0x3C;
            sparkSpec.y = 0x78;
            sparkSpec.z = 0x28;
            t = (ActorStatus_RandomSample(gRandomTable) & 7) + 4;
            lifetime = (ActorStatus_RandomSample(gRandomTable) & 7) + 8;
            SparkParticle_Spawn(&sparkPos, 2, &sparkSpec, rb, t, lifetime, 2, 1);
        }
    }
}

static inline void ActorStatus_FaceTarget(struct ChaseActor *actor)
{
    if (ActorStatus_BodyFlags(actor, 2)) {
        actor->base.tracking.target = (u32)(gUnknown_02000580[0] + 0x24);
    } else if (actor->base.tracking.target == 0) {
        return;
    }
    {
        u8 *tg = (void *)actor->base.tracking.target;
        s32 dx = *(s16 *)(tg + 0xC) - actor->base.body.position.x;
        s32 dy = *(s16 *)(tg + 0x10) - actor->base.body.position.z;

        actor->base.tracking.heading = Math_Atan2(dx, dy);
        actor->base.tracking.distance = dx * dx + dy * dy;
    }
}
#endif
