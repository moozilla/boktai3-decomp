#define SCENE_ACTOR_CHASE_SCALAR_CELLS 1
#define SCENE_ACTOR_CHASE_WIDE_STATE   1
#include "scene_actor_chase_reaction.h"
#include "player.h"
#include "global.h"
#include "render.h"
#include "ram.h"
#include "runtime.h"
#include "engine_globals.h"

extern u32 gLinkSession;
extern u8 *gUnknown_02000580[];

/* Signed countdown interpretation of the canonical raw halfword. */
struct SceneActorPaletteTimer {
    s16 remaining;
} __attribute__((packed, aligned(2)));
typedef char Check_attachmentLimit
    [(u32) & ((struct SceneActorRegistryResetView *)0)->attachmentLimit == 0xA6 ? 1 : -1];
typedef char
    Check_attachmentTimer[(u32) & ((struct SceneActorRegistryResetView *)0)->fieldAA == 0xAA ? 1
                                                                                             : -1];
typedef char Check_alternatePalette
    [(u32) & ((struct SceneActorRegistryResetView *)0)->alternatePalette == 0x1DC ? 1 : -1];
typedef char
    Check_flashTimer[(u32) & ((struct SceneActorRegistryResetView *)0)->field1E8 == 0x1E8 ? 1 : -1];
typedef char Check_deathControl
    [(u32) & ((struct SceneActorRegistryResetView *)0)->deathControl == 0x234 ? 1 : -1];
typedef char Check_secondaryMotion
    [(u32) & ((struct SceneActorRegistryResetView *)0)->secondaryMotion == 0x2EC ? 1 : -1];
typedef char
    Check_particleTimer[(u32) & ((struct SceneActorRegistryResetView *)0)->field30C == 0x30C ? 1
                                                                                             : -1];
typedef char
    Check_route[(u32) & ((struct SceneActorRegistryResetView *)0)->route == 0x448 ? 1 : -1];
typedef char
    Check_contactNode[(u32) & ((struct SceneActorRegistryResetView *)0)->contactNode == 0x45C ? 1
                                                                                              : -1];
typedef char Check_countdown5C8
    [(u32) & ((struct SceneActorRegistryResetView *)0)->countdown5C8 == 0x5C8 ? 1 : -1];
typedef char
    Check_effect5CC[(u32) & ((struct SceneActorRegistryResetView *)0)->effect5CC == 0x5CC ? 1 : -1];
typedef char Check_behaviorControl
    [(u32) & ((struct SceneActorRegistryResetView *)0)->behaviorControl == 0x5DC ? 1 : -1];
typedef char Check_interactionCallback
    [(u32) & ((struct SceneActorRegistryResetView *)0)->interactionCallback == 0x618 ? 1 : -1];
typedef char Check_responseCallback
    [(u32) & ((struct SceneActorRegistryResetView *)0)->responseCallback == 0x61C ? 1 : -1];
struct UpdateRestorationCommand {
    u8 unknown0[8];
    u8 state, phase, behavior, unknownB;
    u32 delay;
    void *control;
};
static inline void ActorStatus_StoreRestoration(struct UpdateRestorationCommand *command,
                                                void *control, u32 state, u32 phase, u32 behavior,
                                                u32 delay)
{
    command->delay = delay;
    command->control = control;
    command->state = state;
    command->phase = phase;
    command->behavior = behavior;
}

static inline void ActorStatus_SetWord(u32 *field, u32 bits)
{
    *field |= bits;
}

static inline void ActorStatus_MaskHalfword(u16 *field, u32 mask)
{
    *field &= mask;
}

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
s32 Nav_IsPositionInMesh(void *, struct ContactPosition *, struct ContactPosition *);
s32 Terrain_ApplyBlocked(void *, struct ContactPosition *, struct ContactPosition *);
u32 TetherActor_CanStartInteraction(void);
s32 Math_Atan2(s32, s32);
void SceneActorReaction_RestoreAfterDelay(void);

static inline u32 ActorStatus_BehaviorFlags(void *object, u32 mask)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->flags2CC & mask;
}
static inline u32 ActorStatus_StatusFlags(void *object, u32 mask)
{
    struct SceneActorRegistryResetView *actor = object;
    return actor->flags2D0 & mask;
}
static inline u32 ActorStatus_BitsPresent(u32 bits)
{
    if (bits)
        return 1;
    return 0;
}
static inline u32 ActorStatus_HasBehaviorFlags(void *object, u32 mask)
{
    return ActorStatus_BitsPresent(ActorStatus_BehaviorFlags(object, mask));
}
static inline u32 ActorStatus_HasStatusFlags(void *object, u32 mask)
{
    return ActorStatus_BitsPresent(ActorStatus_StatusFlags(object, mask));
}

static inline u16 ActorStatus_MotionFlags(void *object, u16 mask)
{
    struct SceneActorRegistryResetView *actor = object;
    return SceneActorChase_FlagComponentBits((void *)&actor->flags2D4, mask);
}
static inline u8 ActorStatus_BodyFlags(struct SceneActorRegistryResetView *actor, u32 mask)
{
    if (actor->flags2C0 & mask)
        return 1;
    return 0;
}
static inline void ActorStatus_ClearTrackingFlags(struct SceneActorReactionMotion *tracking,
                                                  u32 bits)
{
    u32 mask = ~bits;
    tracking->flags &= mask;
}
static inline s16 ActorStatus_FlashRemaining(struct SceneActorRegistryResetView *actor)
{
    return (*(s16 *)&actor->field1E8);
}
static inline s16 ActorStatus_BlinkRemaining(struct SceneActorRegistryResetView *actor)
{
    return ((struct SceneActorPaletteTimer *)&actor->field1EA)->remaining;
}
static inline void ActorStatus_SetPlacementFlags(struct SceneActorChasePlacement *placement,
                                                 u32 bits)
{
    placement->flags |= bits;
}
static inline void ActorStatus_ClearPlacementFlags(struct SceneActorChasePlacement *placement,
                                                   u32 bits)
{
    u32 mask = ~bits;
    placement->flags &= mask;
}
static inline void ActorStatus_StoreByte(u8 *field, u8 value)
{
    *field = value;
}

/* actor kind 0: palette bank via AnimData; otherwise a raw bank pointer */
static inline void ActorStatus_SetPalette(struct SceneActorRegistryResetView *actor, u32 idx)
{
    if (actor->drawableKind == 0)
        AnimData_SelectPaletteBank((u8 *)actor->drawable + 0x2C, idx);
    else
        *(u8 **)((u8 *)actor->drawable + 0x68) = gPaletteBankData + (idx << 5);
}

/* actor exists and is dead/disabled */
static inline u8 ActorStatus_IsDown(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    if (actor == 0)
        return 0;
    if (ActorStatus_MotionFlags((void *)actor, 0x40) ||
        ActorStatus_HasStatusFlags(actor, 0x20000) || actor->health <= 0)
        return 1;
    return 0;
}

static inline void ActorStatus_SetDrawableFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    if (actor->drawableKind == 0)
        ((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->flags |= bits;
    else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
        tile->flags |= bits;
    }
}
static inline void ActorStatus_ClearTileFlags(struct SceneActorAnimationTile *tile, u32 bits)
{
    u32 keep = ~bits;
    tile->flags &= keep;
}

static inline void ActorStatus_ClearDrawableFlags(struct SceneActorRegistryResetView *actor,
                                                  u32 bits)
{
    if (actor->drawableKind == 0)
        SceneActorChase_ClearWordFlags(
            &((struct SceneActorRegistrySequenceDrawable *)actor->drawable)->flags, bits);
    else {
        struct SceneActorAnimationTile *tile = (void *)((u8 *)actor->drawable + 0x20);
        ActorStatus_ClearTileFlags(tile, bits);
    }
}

static inline void ActorStatus_SetBodyBit(struct SceneActorRegistryResetView *actor, u32 on)
{
    if (on)
        ActorStatus_SetDrawableFlags(actor, 1);
    else
        ActorStatus_ClearDrawableFlags(actor, 1);
}

static inline void ActorStatus_ReleaseContact(struct SceneActorRegistryResetView *actor)
{
    u32 flag = 0x1000;
    if (actor->flags2D4 & flag) {
        SceneActorContacts_ReleaseNode(actor->contactNode);
        actor->flags2D4 &= ~flag;
        actor->contactNode = 0;
    }
}

static inline void ActorStatus_ClearMotionFlags(struct SceneActorRegistryResetView *actor, u32 bits)
{
    actor->flags2D4 &= ~bits;
}

static inline void ActorStatus_ResetInteractionAt(struct SceneActorRegistryResetView *actor,
                                                  struct SceneActorRegistryControl *control)
{
    if (ActorStatus_HasBehaviorFlags(actor, 0x200000)) {
        SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x1000000);
        ActorStatus_ClearMotionFlags(actor, 0x200);
        control->mode = 0;
        control->counter = 0;
        control->timer = 0;
    }
}
static inline void ActorStatus_ResetInteraction(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    ActorStatus_ResetInteractionAt(actor, (void *)&actor->group.control);
}
static inline void ActorStatus_ResetRouteAt(struct SceneActorRegistryResetView *actor,
                                            struct SceneActorRegistryControl *control)
{
    SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x141C0);
    control->counter = 0;
    control->field14 = 0;
    control->timer = 0;
}
static inline void ActorStatus_ResetRoute(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    ActorStatus_ResetRouteAt(actor, (void *)&actor->group.control);
}
static inline void ActorStatus_ResetSelection(struct SceneActorRegistryResetView *actor)
{
    actor->field5DA = 255;
}

/* shared reset of the knockback/stun scratch state */
static inline void ActorStatus_ResetReaction(struct SceneActorRegistryResetView *actor)
{
    actor->field2C2 = 0x40;
    actor->group.flags414 = 0;
    actor->group.field418 = 0;
    ActorStatus_ClearMotionFlags(actor, 0x380);
    ActorStatus_ClearTrackingFlags((void *)&actor->fieldDC, 0x18);
    ActorStatus_ResetInteraction(actor);
    ActorStatus_ClearMotionFlags(actor, 0x400);
    ActorStatus_ResetSelection(actor);
}

/* the dead/downed behaviour block: set action table slots */

/* terrain cell index from a packed position pair */
static inline void ActorStatus_TickRouteControl(struct SceneActorRegistryControl *control)
{
    if (control->timer)
        control->timer--;
}
static inline void ActorStatus_TickAttachment(struct SceneActorRegistryResetView *actor)
{
    s32 remaining = (*(s16 *)&actor->fieldAA);
    if (remaining <= 0) {
        (*(s16 *)&actor->fieldAA) = 0;
        if (ActorStatus_HasStatusFlags(actor, 0xC0))
            ActorStatus_SetWord(&actor->flags2D0, 0x100);
    } else {
        (*(s16 *)&actor->fieldAA) = remaining - 1;
    }
    if (actor->countdown5C8)
        actor->countdown5C8--;
}

static inline u32 ActorStatus_IsUnstable(struct SceneActorRegistryResetView *actor)
{
    u32 result = 0;
    if (ActorStatus_BodyFlags(actor, 0x1000))
        return 1;
    if (ActorStatus_MotionFlags(actor, 0xA03F) || actor->colliderA.responseValue != 0 ||
        (*(s16 *)&actor->field2EA) > 0 || (*(u32 *)&actor->secondaryMotion.x) != 0 ||
        actor->secondaryMotion.z != 0)
        return 1;
    if (ActorStatus_IsDown(actor))
        result = 1;
    return result;
}
static inline void ActorStatus_CollisionGate(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = (void *)&actor->group;
    if (ActorStatus_BodyFlags(actor, 0x200)) {
        if ((ActorStatus_IsUnstable(actor) && !ActorStatus_MotionFlags(actor, 0x8000)) ||
            ActorStatus_IsDown(actor))
            ActorStatus_SetPlacementFlags(&group->placement, 1);
        else
            ActorStatus_ClearPlacementFlags(&group->placement, 1);
    }
}

/* Current scene mode and cached environmental level. Each query returns
 * a value snapshot; consumers select the property required by their operation. */
struct SceneEnvironmentSnapshot {
    u32 linkMode;
    s32 solarLevel;
};
static inline struct SceneEnvironmentSnapshot SceneEnvironment_Read(void)
{
    struct SceneEnvironmentSnapshot result;
    result.linkMode = SceneActorChase_Test(gEngineFlags, 0x4800);
    result.solarLevel = *(s16 *)((u8 *)gGameState + 0x876);
    return result;
}
static inline s32 SceneEnvironment_SolarLevel(void)
{
    struct SceneEnvironmentSnapshot result = SceneEnvironment_Read();
    return result.solarLevel;
}
static inline u32 SceneEnvironment_LinkMode(void)
{
    struct SceneEnvironmentSnapshot result = SceneEnvironment_Read();
    return result.linkMode;
}

static inline void ActorStatus_EffectSelector(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorChaseGroup *group = (void *)&actor->group;
    s32 v;
    /* region marker at +0x5CC based on level */
    if (ActorStatus_HasBehaviorFlags(actor, 0x40000)) {
        v = SceneEnvironment_SolarLevel();
        if (v <= 2) {
            if (ActorStatus_HasBehaviorFlags(actor, 0x80000))
                group->effect = 0x21;
            else
                group->effect = 0x33;
        } else if (v <= 6) {
            group->effect = 0x32;
        } else {
            if (ActorStatus_HasBehaviorFlags(actor, 0x80000))
                group->effect = 0x33;
            else
                group->effect = 0x21;
        }
    }
}

static inline u32 ActorStatus_DefaultPalette(struct SceneActorRegistryResetView *actor)
{
    u16 *base = &actor->basePaletteBank;
    u16 *offset = &actor->paletteOffset;
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

static inline void ActorStatus_SetBlinkTimer(struct SceneActorRegistryResetView *actor, s16 timer)
{
    ((struct SceneActorPaletteTimer *)&actor->field1EA)->remaining = timer;
}

static inline void ActorStatus_Palette(struct SceneActorRegistryResetView *actor)
{
    s16 *pTmr;

    u32 res;
    /* palette blink / flash */
    if (ActorStatus_BehaviorFlags(actor, 0x8000)) {
        s32 flashRemaining;
        if (ActorStatus_BehaviorFlags(actor, 0x2000000)) {
            s32 blinkRemaining = ActorStatus_BlinkRemaining(actor);
            if (blinkRemaining > 0) {
                ((struct SceneActorPaletteTimer *)&actor->field1EA)->remaining = blinkRemaining - 1;
            } else {
                ActorStatus_SetBlinkTimer(actor, 0);
                SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x2000000);
            }
        }
        flashRemaining = ActorStatus_FlashRemaining(actor);
        pTmr = &(*(s16 *)&actor->field1E8);
        if (flashRemaining > 0) {
            *pTmr = flashRemaining - 1;
            return;
        }
        if ((ActorStatus_BehaviorFlags(actor, 0x2000000)) ||
            (ActorStatus_HasStatusFlags(actor, 0x40))) {
            if (ActorStatus_HasStatusFlags(actor, 0x4000))
                ActorStatus_SetPalette(actor, actor->alternatePalette);
            else
                ActorStatus_SetPalette(actor, actor->alternateOffset);
        } else {
            ActorStatus_SetPalette(actor, ActorStatus_DefaultPalette(actor));
        }
        *pTmr = 0;
        SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x8000);
        return;
    } else if (ActorStatus_BehaviorFlags(actor, 0x2000000)) {
        s32 blinkRemaining;
        s32 phase;
        blinkRemaining = ActorStatus_BlinkRemaining(actor);
        pTmr = &((struct SceneActorPaletteTimer *)&actor->field1EA)->remaining;
        if (blinkRemaining > 0) {
            if (blinkRemaining <= 0x40) {
                if (blinkRemaining >= 0)
                    phase = blinkRemaining >> 2;
                else
                    phase = -((-blinkRemaining) >> 2);
                if (phase & 1) {
                    ActorStatus_SetPalette(actor, ActorStatus_DefaultPalette(actor));
                } else if (ActorStatus_HasStatusFlags(actor, 0x4000)) {
                    ActorStatus_SetPalette(actor, actor->alternatePalette);
                } else {
                    ActorStatus_SetPalette(actor, actor->alternateOffset);
                }
            } else {
                if (ActorStatus_HasStatusFlags(actor, 0x4000))
                    ActorStatus_SetPalette(actor, actor->alternatePalette);
                else
                    ActorStatus_SetPalette(actor, actor->alternateOffset);
            }
            *pTmr = *pTmr - 1;
        } else {
            ActorStatus_SetPalette(actor, ActorStatus_DefaultPalette(actor));
            *pTmr = 0;
            SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x2000000);
            SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x4000);
        }
    } else if (ActorStatus_HasStatusFlags(actor, 0xC0)) {
        if (ActorStatus_HasStatusFlags(actor, 0x100)) {
            SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x4140);
            if (ActorStatus_HasStatusFlags(actor, 0x80)) {
                ActorStatus_SetWord(&actor->flags2CC, 0x2000000);
                SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x80);
                ActorStatus_SetBlinkTimer(actor, 0x40);
                return;
            }
            ActorStatus_SetPalette(actor, ActorStatus_DefaultPalette(actor));
            ActorStatus_SetBlinkTimer(actor, 0);
        } else {
            s32 recoveryRemaining;
            s32 phase;
            recoveryRemaining = ActorStatus_BlinkRemaining(actor);
            pTmr = &((struct SceneActorPaletteTimer *)&actor->field1EA)->remaining;
            if (recoveryRemaining >= 0) {
                if (recoveryRemaining <= 0x40) {
                    if (recoveryRemaining >= 0)
                        phase = recoveryRemaining >> 2;
                    else
                        phase = -((-recoveryRemaining) >> 2);
                    if (phase & 1) {
                        ActorStatus_SetPalette(actor, ActorStatus_DefaultPalette(actor));
                    } else if (ActorStatus_HasStatusFlags(actor, 0x4000)) {
                        ActorStatus_SetPalette(actor, actor->alternatePalette);
                    } else {
                        ActorStatus_SetPalette(actor, actor->alternateOffset);
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
            u32 current = actor->paletteBank;
            u32 restored = ActorStatus_DefaultPalette(actor);
            SceneActorPalette_BeginTransition(current, restored, 0, actor->category, 0);
            ActorStatus_SetWord(&actor->flags2CC, 0x20000);
        }
    } else if (ActorStatus_BehaviorFlags(actor, 0x20000)) {
        SceneActorPalette_BeginTransition((ActorStatus_DefaultPalette(actor)), actor->paletteBank,
                                          0, actor->category, 1);
        SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x20000);
    }
}

static inline s16 ActorStatus_AttachmentRestart(struct SceneActorRegistryResetView *actor)
{
    return actor->fieldA8;
}

static inline void ActorStatus_Wake(struct SceneActorRegistryResetView *actor)
{
    const struct SceneActorRegistryActor *identity = (void *)actor;

    /* wake-up transition when the stun flag is set */
    if (!(ActorStatus_MotionFlags(actor, 0x40)) && !(ActorStatus_StatusFlags(actor, 0x20000)) &&
        (ActorStatus_BehaviorFlags(actor, 0x1000))) {
        ActorStatus_ResetRoute(actor);
        SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x1000);
        actor->reactionStart.x = actor->position.x;
        actor->reactionStart.z = actor->position.z;
        actor->reactionEnd.x = actor->position.x;
        actor->reactionEnd.z = actor->position.z;
        if ((*(s16 *)&actor->fieldAA) >= actor->attachmentLimit)
            (*(s16 *)&actor->fieldAA) = ActorStatus_AttachmentRestart(actor);
        if (actor->health <= 0) {
            SceneActorChase_SetState(actor, 3, 0, 0);
            SceneActorChase_SetControl(actor, actor->deathControl, 0x11);
            if (!SceneEnvironment_LinkMode() && !ActorStatus_StatusFlags((void *)identity, 0x800))
                SceneActorRecord_RecordPair(
                    gScriptEntryProcedure,
                    (u16)((const struct SceneActorRegistryActor *)identity)->id);
        } else if (ActorStatus_BehaviorFlags(actor, 0x200)) {
            SceneActorChase_SetControl(actor, actor->reactionTarget230, 0x12);
            SceneActorChase_SetState(actor, 3, 0, 0);
        } else {
            SceneActorAttachment_ReplaceResponseSlot(actor, 3, 0x7F, actor->category);
            Callback_InvokeActor(actor, actor->responseCallback);
            SceneActorChase_SetControl(actor, actor->behaviorControl, 7);
            SceneActorChase_SetState(actor, 3, 0, 0);
        }
    }
}

static inline u16 ActorStatus_TickResponse(struct ContactCollider *response)
{
    return --response->responseValue;
}

static inline void ActorStatus_Blink(struct SceneActorRegistryResetView *actor)
{

    /* body bit and shadow follow the blink timer at +0x178 */
    if (!(ActorStatus_MotionFlags(actor, 0x40)) && !(ActorStatus_StatusFlags(actor, 0x20000))) {
        struct ContactCollider *response = &actor->colliderA;
        if (actor->health <= 0) {
            if (actor->colliderA.responseValue != 0) {
                actor->colliderA.responseValue = 0;
                ActorStatus_SetBodyBit(actor, 0);
                if (actor->shadow[0] != 0)
                    ActorShadow_Enable((u8 *)actor + 0x320);
            }
        } else if (actor->colliderA.responseValue != 0 && !(ActorStatus_MotionFlags(actor, 0x20))) {
            if ((actor->colliderA.responseValue >> 2) & 1) {
                ActorStatus_SetBodyBit(actor, 0);
                if (actor->shadow[0] != 0)
                    ActorShadow_Enable((u8 *)actor + 0x320);
            } else {
                ActorStatus_SetBodyBit(actor, 1);
                if (actor->shadow[0] != 0)
                    ActorShadow_Disable((u8 *)actor + 0x320);
            }
            if (ActorStatus_TickResponse(response) == 0) {
                ActorStatus_SetBodyBit(actor, 0);
                if (actor->shadow[0] != 0)
                    ActorShadow_Enable((u8 *)actor + 0x320);
            }
        }
    }
}

static inline void ActorStatus_SetPendingBits(struct SceneActorRegistryResetView *actor, u16 bits)
{
    actor->flags2D6 |= bits;
}

static inline void ActorStatus_SetRecoveryDuration(struct SceneActorRegistryResetView *actor,
                                                   u32 category)
{
    if (category == 0x15 || category == 0x19) {
        actor->field2C2 = 0x20;
        return;
    }
    if (category - 5 <= 2) {
        actor->field2C2 = 0x20;
        return;
    }
    actor->field2C2 = Div(0x40, 3);
}

static inline void ActorStatus_ReactionFlags(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = (void *)&actor->group;
    u32 cat;
    /* state-flag dispatch */
    if (((struct SceneActorRegistryResetView *)object)->flags2D4 == 0) {
        SceneActorChase_ClearWordFlags(&((struct SceneActorRegistryResetView *)object)->flags2CC,
                                       0x2000);
    } else if (ActorStatus_IsDown(object)) {
        ActorStatus_ReleaseContact(object);
        ActorStatus_ResetInteractionAt(actor, (void *)((u8 *)group + 0x4C));
        actor->flags2D4 = 0x40;
        actor->field2E6 = 0;
        SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
    } else {

        cat = actor->category;

        if (ActorStatus_MotionFlags(actor, 0x2000)) {
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearTrackingFlags((void *)&actor->fieldDC, 8);
            ActorStatus_ClearMotionFlags(actor, 0x500);
            ActorStatus_SetPendingBits(actor, 0x400);
            actor->field2C2 = 0x40;
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 4)) {
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearTrackingFlags((void *)&actor->fieldDC, 8);
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            actor->field2C2 = 0x40;
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
            ActorStatus_SetWord(&actor->flags2CC, 0x2000);
        } else if (ActorStatus_MotionFlags(actor, 2)) {
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearTrackingFlags((void *)&actor->fieldDC, 8);
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 0x8000)) {
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 0x10)) {
            ActorStatus_ResetRouteAt(actor, (void *)((u8 *)group + 0x4C));
            ActorStatus_ResetReaction(object);
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            (*(s16 *)&actor->fieldAA) = 0;
            actor->field2C2 = Div(0x40, 6);
            ActorStatus_ReleaseContact(object);
            ActorStatus_ResetInteractionAt(actor, (void *)((u8 *)group + 0x4C));
            ActorStatus_ClearMotionFlags(actor, 0x400);
            group->turn.turnsRemaining |= -1;
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
        } else if (ActorStatus_MotionFlags(actor, 8)) {
            ActorStatus_ResetRouteAt(actor, (void *)((u8 *)group + 0x4C));
            ActorStatus_ResetInteractionAt(actor, (void *)((u8 *)group + 0x4C));
            ActorStatus_ClearMotionFlags(actor, 0x2500);
            ActorStatus_SetPendingBits(actor, 0x400);
            SceneActorChase_ClearBits(&actor->flags2D6, 0x980);
        }

        if (ActorStatus_MotionFlags(actor, 0x803E)) {
            ActorStatus_ClearMotionFlags(actor, 0x201);
            SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x4000);
            ActorStatus_ClearTrackingFlags((void *)&actor->fieldDC, 0x10);
            if (!(ActorStatus_MotionFlags(actor, 0x20)))
                SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x80);
            SceneActorChase_ClearWordFlags(&actor->flags2D0, 0x100004);
            ActorStatus_ReleaseContact(object);
            ActorStatus_ResetInteractionAt(actor, (void *)((u8 *)group + 0x4C));
            ActorStatus_ClearMotionFlags(actor, 0x400);
            group->turn.turnsRemaining = 0xFF;
            ActorStatus_SetPendingBits(actor, 0x400);
            actor->field2E6 = 0;
            if (!(SceneActorChase_ReadMotionFlags(actor) & 0x10))
                ActorStatus_SetWord(&actor->flags2CC, 0x2000);
            else
                SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x2000);
        } else {
            SceneActorChase_ClearWordFlags(&actor->flags2CC, 0x2000);
        }

        if (ActorStatus_MotionFlags(actor, 0x400)) {
            if (group->turn.turnsRemaining < 0 ||
                (actor->field2B5 != 0x1B && actor->field2B5 != 2)) {
                group->turn.turnsRemaining = 0xFF;
                ActorStatus_ClearMotionFlags(actor, 0x400);
            }
        }
        if (ActorStatus_MotionFlags(actor, 0x80)) {
            ActorStatus_SetRecoveryDuration(actor, cat);
        } else if (ActorStatus_MotionFlags(actor, 0x1200)) {
            ActorStatus_SetRecoveryDuration(actor, cat);
        } else {
            ActorStatus_ClearMotionFlags(actor, 0x400);
            group->flags = 0;
            group->turn.turnsRemaining = 0xFF;
            group->turn.direction = 0;
            group->turn.savedDirection = 0;
        }
    }
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

static inline void ActorStatus_Particles(struct SceneActorRegistryResetView *actor)
{
    u32 lifetime;
    s32 t;
    struct ContactPackedVector sparkSpec;
    union {
        struct ContactPosition position;
        struct ContactPackedVector packed;
    } sparkPos;
    /* flicker spark while the hit timer runs */
    if ((*(u16 *)&actor->field30C) != 0) {
        (*(u16 *)&actor->field30C)--;
        if (actor->position.y >= 0 && Mod((*(u16 *)&actor->field30C), 4) == 0) {
            u8 baseAngle = actor->field2B6;
            u32 rb;

            sparkPos.position = actor->position;
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

static inline u8 ActorStatus_TryRecoveryRoute(struct SceneActorRegistryResetView *actor,
                                              void *route, struct ContactPosition *from,
                                              struct ContactPosition *to)
{
    s32 blocked;
    if (!Nav_IsPositionInMesh(route, from, to)) {
        ActorStatus_ClearMotionFlags(actor, 0x200);
        return 0;
    }
    blocked = Terrain_ApplyBlocked(route, from, to);
    if (blocked) {
        if (blocked != 2)
            Memory_Clear(&actor->motion, sizeof(actor->motion));
        return 1;
    }
    ActorStatus_ClearMotionFlags(actor, 0x200);
    return 0;
}

static inline void ActorStatus_RecoverAndRelease(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    struct SceneActorChaseGroup *group = (void *)&actor->group;
    u32 cat;
    s32 t;
    u32 res;
    struct ContactPosition targetPosition;
    if ((ActorStatus_MotionFlags(object, 0x80)) && !(ActorStatus_MotionFlags(object, 0xA03F)) &&
        ((struct SceneActorRegistryResetView *)object)->colliderA.responseValue == 0) {
        if (((struct SceneActorRegistryResetView *)object)->group.field418 != 0) {
            ((struct SceneActorRegistryResetView *)object)->group.field418--;
        } else if ((cat = ((struct SceneActorRegistryResetView *)object)->category) != 2 &&
                   cat != 0x19 && !(ActorStatus_MotionFlags(object, 0x100))) {
            u8 *grp;
            u8 *r5;

            ActorStatus_ResetReaction(object);
            SceneActorAttachment_ReplaceResponseSlot(object, 0, 2, cat);
            SceneActorAnimation_SelectDirectionalEntry(object, 0);
            SceneActorChase_SetControl(object, (void *)SceneActorReaction_RestoreAfterDelay, 0xE);
            SceneActorChase_SetState(object, 3, 1, 0);
            ActorStatus_StoreRestoration(
                (void *)&((struct SceneActorRegistryResetView *)object)->word460,
                ((struct SceneActorRegistryResetView *)object)->entryTarget228, 0, 0, 7, 60);
            ActorStatus_SetWord(&((struct SceneActorRegistryResetView *)object)->flags2CC, 0x10);
            ActorStatus_MaskHalfword(&((struct SceneActorRegistryResetView *)object)->flags2D6,
                                     (u32)(s16)0xF636);

            SceneActorChase_SnapToRoute(
                &targetPosition, (void *)&((struct SceneActorRegistryResetView *)object)->route);

            group->destination->pad = 1;
            grp = group->route;
            r5 = (u8 *)actor + 0x5C;
            res = ActorStatus_TryRecoveryRoute(object, grp, (void *)r5, &targetPosition);
            if (!res)
                Callback_InvokeActor(actor, group->fallback);
        }
    }
    if ((ActorStatus_MotionFlags(object, 0x1000)) && !TetherActor_CanStartInteraction())
        Callback_InvokeActor(object,
                             ((struct SceneActorRegistryResetView *)object)->interactionCallback);
}
static inline void ActorStatus_FaceTarget(struct SceneActorRegistryResetView *actor)
{
    if (ActorStatus_BodyFlags(actor, 2)) {
        actor->fieldDC = (u32)(gUnknown_02000580[0] + 0x24);
    } else if (actor->fieldDC == 0) {
        return;
    }
    {
        u8 *tg = (void *)actor->fieldDC;
        s32 dx = *(s16 *)(tg + 0xC) - actor->position.x;
        s32 dy = *(s16 *)(tg + 0x10) - actor->position.z;

        actor->fieldE9 = Math_Atan2(dx, dy);
        actor->distanceF0 = dx * dx + dy * dy;
    }
}

s32 SceneActor_UpdateStatus(struct SceneActorRegistryActor *actor)
{
    struct SceneActorRegistryActor *arg0 = (void *)actor;
    if (ActorStatus_StatusFlags(arg0, 0x2000))
        ((struct SceneActorRegistryResetView *)arg0)->field2C2 = 0x15;
    ActorStatus_TickRouteControl(&((struct SceneActorRegistryResetView *)arg0)->group.control);
    if (!ActorStatus_IsDown(arg0))
        ActorStatus_TickAttachment((void *)actor);
    ActorStatus_CollisionGate(actor);
    ActorStatus_EffectSelector((void *)arg0);
    ActorStatus_Palette((void *)arg0);
    ActorStatus_Wake((void *)actor);
    ActorStatus_Blink((void *)arg0);
    ActorStatus_ReactionFlags(actor);
    ActorStatus_Particles((void *)arg0);
    if (!ActorStatus_IsDown(arg0)) {
        ActorStatus_RecoverAndRelease(actor);
        ActorStatus_FaceTarget((void *)arg0);
    }
    ActorStatus_StoreByte(&((struct SceneActorRegistryResetView *)arg0)->colliderB.direction,
                          ((struct SceneActorRegistryResetView *)arg0)->direction);
    ActorStatus_StoreByte(&((struct SceneActorRegistryResetView *)arg0)->colliderA.direction,
                          ((struct SceneActorRegistryResetView *)arg0)->direction);
    return 1;
}
