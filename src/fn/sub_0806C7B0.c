#include "scene_steering_actor.h"
#include "player_cue_sequence.h"
struct SceneSteeringActorPool *SceneSteeringActor_EnsurePool(void);
struct SceneSteeringActor *SceneSteeringActor_Allocate(struct SceneSteeringActorPool *);
void SceneSteeringActor_InitializeCollider(struct SceneSteeringActor *, s32, s32, s32, s32, s32,
                                           s32, s32);
u32 ScaledShadow_Initialize(void *, const struct ContactPosition *, s32, s32, s32, s32, s32, s32,
                            s32, s32, s32);
void ScaledShadow_SetGraphic(void *, s32, s32);
void ScaledShadow_SetFlag(void *, s32);
void ScaledShadow_Disable(void *);
void SceneSteeringActor_CreateTravelActor(const struct ContactPosition *position,
                                          const struct ContactPosition *target, s32 velocityY,
                                          s32 direction, s32 steeringTimer, s32 turnStep,
                                          s32 steeringDelay, s32 speed, s32 speedLimit, s32 a,
                                          s32 b, s32 c, s32 d)
{
    struct SceneSteeringActorPool *pool;
    struct SceneSteeringActor *slot;
    struct SceneSteeringActor *sprite;
    pool = gSceneSteeringActorPool;
    if (!pool) {
        pool = SceneSteeringActor_EnsurePool();
        if (!pool)
            return;
    }
    slot = SceneSteeringActor_Allocate(pool);
    if (!slot)
        return;
    slot->state = 1;
    slot->lifetime = 120;
    slot->flags120 = 0;
    slot->initialDirection = direction;
    slot->target = *target;
    slot->direction = direction;
    slot->steeringTimer = steeringTimer;
    slot->turnStep = turnStep;
    slot->steeringDelay = steeringDelay;
    slot->speed = speed;
    slot->parameter = speedLimit;
    slot->enter = 1;
    ZeroFill8(&slot->velocity);
    slot->velocity.y = velocityY;
    sprite = slot;
    slot->position = *position;
    slot->sequenceResource = ScriptProc_Start(0x922E, 0x121B);
    Sprite_Init(slot, pool->graphics, 0);
    Anim_SetSequence(&slot->sequence, (void *)slot->sequenceResource, 7, 0, 0);
    AnimData_SelectPaletteBank(slot->graphics, 0x127);
    PlayerCue_StepSequence((struct ContactSwarmSprite *)slot, &slot->sequence);
    sprite->priority = 2;
    sprite->flags &= ~1;
    SceneSteeringActor_InitializeCollider(slot, a, b, c, d, 0, 0, 0);
    slot->shadowStatus =
        ScaledShadow_Initialize(slot->shadow, &slot->position, 64, 64, 64, 100, 0, 0, 5, 1, 0);
    ScaledShadow_SetGraphic(slot->shadow, 0x1C1B, 6);
    ScaledShadow_SetFlag(slot->shadow, 0);
    ScaledShadow_Disable(slot->shadow);
}
