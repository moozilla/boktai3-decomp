#include "global.h"
#include "engine/engine.h"
#include "scene/terrain/terrain_cell_probe.h"
#include "scene/motion/scene_actor_charge_offset.h"
#include "npc/story_character/state_actor.h"
#include "scene/collision/contact_position.h"
#include "scene/terrain/terrain_sampling.h"
#include "scene/actor/scene_actors.h"
#include "bosses/count/scene_graphics_actor.h"

u32 SceneActorGraphics_GetResourceId(void *);
void SceneActors_StopBackgroundFlash(void *);
void RouteSpritePool_RequestFinishAll(void);
void WirelessGame_NotifyOpponentDefeated(void);
void sub_08022790(void *, s32);
void sub_08022878(void *);
void Bestiary_UnlockEntry(s32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);

struct __attribute__((packed)) DeathState {
    u8 value;
};

static inline s32 sub_08027214_Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline u32 Death_GetTerrainHeight(struct Pos16 *position)
{
    u8 *data = cellProps(cellIndexAt(position));
    u32 kind = *data >> 4;
    u32 height = (*data & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

static inline s32 Death_CellBlocks(struct Pos16 *position)
{
    u8 *data = cellProps(cellIndexAt(position));
    u16 flags = *(u16 *)(data + 2);
    flags &= 1;
    return flags;
}

static inline void Death_SetVelocity(struct ContactPosition *vector, s32 angle, s32 speed)
{
    {
        s32 x = sub_08027214_Scale(gUnk_086149C4[(angle + 0x40) & 0xff] * speed);
        vector->x = x;
        vector->y = 0;
    }
    vector->z = sub_08027214_Scale(gUnk_086149C4[angle & 0xff] * speed);
}

static inline u32 Death_GetMode(struct SceneGraphicsActor *actor)
{
    return actor->graphics.animationFlags;
}

static inline void Death_SetState(struct DeathState *out, struct DeathState state)
{
    *out = state;
}

void sub_08027214(struct StateActor *base, s32 tick)
{
    struct SceneGraphicsActor *actor = (struct SceneGraphicsActor *)base;
    u8 *pos;

    if (base->actionChanged) {
        base->actionChanged = 0;
        SceneActors_RunFirstEventScript(base, &actor->animation.position);
        if (actor->graphics.stepTableKind == 1) {
            s32 angle = actor->hitAngle + 0x80;
            struct DeathState state;
            s32 sequence;
            base->bodyDirection = angle;
            {
                s32 direction = (base->bodyDirection + 0x60) >> 6;
                u32 mask = 3;
                mask &= direction;
                base->animationFacing4 = mask;
            }
            {
                s32 direction = (base->bodyDirection + 0x70) >> 5;
                state.value = 7;
                direction = (u8)direction & state.value;
                base->animationFacing8 = direction;
            }
            sequence = (actor->graphics.animationFlags & 1) ? 0xB : 0x31;
            SceneActors_StartDirectionalAnimation(base, &actor->animation, &actor->animationResource, sequence, 2, 1,
                                                  4);
            Death_SetState((struct DeathState *)&base->reactionPhase, state);
            base->reactionTimer = 0;
            Sound_PlaySongChecked(0x359);
        } else {
            u32 flash = Death_GetMode(actor) & 1;
            s32 sequence = flash ? 0x20 : 0x46;
            SceneActors_StartUnmirroredAnimation(base, &actor->animation, &actor->animationResource, sequence, 1, 0);
            actor->graphics.scaleMin = 0;
            actor->graphics.scaleMax = 0;
            actor->variant = 0;
            if (actor->graphics.animationFlags & 1) {
                SceneActorState_EnterReactionPhase(base, 1);
                Sound_PlaySongChecked(0x1C5);
            } else {
                SceneActorState_EnterReactionPhase(base, 0);
                Sound_PlaySongChecked(0x359);
            }
        }
        {
            u32 resource = SceneActorGraphics_GetResourceId(actor);
            u32 mode = 2;
            u32 step = 1;
            actor->graphics.resourceId = resource;
            actor->graphics.effectMode = mode;
            actor->graphics.effectDuration = step;
        }
        if (actor->graphics.effectMode == 2) {
            actor->graphics.effectScale = 2;
        } else if (actor->graphics.effectMode == 1) {
            actor->graphics.effectScale = 0;
        }
        actor->graphics.retreatFrames = 0;
        RouteSpritePool_RequestFinishAll();
        SceneActors_StopBackgroundFlash(actor);
        if (gEngineFlags & 0x1000) {
            WirelessGame_NotifyOpponentDefeated();
        }
    }

    if (Death_GetTerrainHeight((struct Pos16 *)&base->target) < (u32)actor->graphics.recoveryGroundHeight ||
        Death_CellBlocks((struct Pos16 *)&base->target)) {
        actor->graphics.retreatFrames = 8;
    }

    pos = (u8 *)&base->target;
    if (actor->graphics.retreatFrames) {
        actor->graphics.retreatFrames--;
        {
            s16 *origin = &actor->retreatTarget.x;
            s32 dx = origin[0] - *(s16 *)(pos + 0);
            s32 dy = origin[2] - *(s16 *)(pos + 4);
            s32 angle = Math_Atan2(dx, dy);
            Death_SetVelocity((struct ContactPosition *)base->motion, angle, 20);
        }
    }

    switch (base->reactionPhase) {
        case 0:
            actor->graphics.reactionDelay = 8;
            if (base->animationCompletions > 4) {
                SceneActorState_EnterReactionPhase(base, 2);
                SceneActors_StartUnmirroredAnimation(base, &actor->animation, &actor->animationResource, 0x21, 2, 0);
                Sound_PlaySongChecked(0x350);
            }
            break;
        case 1:
            if ((tick & 3) == 0) {
                sub_08022790(actor, 1);
            }
            actor->graphics.reactionDelay = 8;
            if (base->animationCompletions > 4) {
                SceneActorState_EnterReactionPhase(base, 2);
                SceneActors_StartUnmirroredAnimation(base, &actor->animation, &actor->animationResource, 0x21, 2, 0);
                actor->graphics.animationFlags &= ~1;
                Sound_PlaySongChecked(0x359);
            }
            break;
        case 2:
            if (actor->animation.frameIndex == 1 && actor->animation.elapsed == 0) {
                Sound_PlaySongChecked(0x350);
            }
            if (actor->animation.status & 2) {
                SceneActorState_EnterReactionPhase(base, 3);
                SceneActors_StartUnmirroredAnimation(base, &actor->animation, &actor->animationResource, 0x22, 2, 0);
                sub_08022878(&actor->animation.position);
            }
            break;
        case 3:
            if (actor->animation.status & 2) {
                SceneActorState_EnterReactionPhase(base, 4);
                SceneActors_StartUnmirroredAnimation(base, &actor->animation, &actor->animationResource, 0x23, 2, 0);
            }
            break;
        case 4:
            if (actor->animation.frameIndex == 0xA && actor->animation.elapsed == 0) {
                Sound_PlaySongChecked(0x354);
            }
            if (actor->animation.status & 2) {
                SceneActorState_EnterReactionPhase(base, 5);
                SceneActors_StartUnmirroredAnimation(base, &actor->animation, &actor->animationResource, 0x24, 2, 0);
            }
            break;
        case 5:
            if (actor->animation.status & 2) {
                SceneActorState_EnterReactionPhase(base, 6);
                Bestiary_UnlockEntry(0x4D);
                SceneActors_RunSecondEventScript(base, &actor->animation.position);
            }
            break;
        case 6:
            break;
        case 7:
            actor->graphics.reactionDelay = 8;
            if (base->reactionTimer > 0xEF) {
                SceneActorState_EnterReactionPhase(base, 5);
            }
            break;
        default:
            break;
    }
    base->reactionTimer += 1;
    SceneActorState_SetFlags(base, 0xD);
}
