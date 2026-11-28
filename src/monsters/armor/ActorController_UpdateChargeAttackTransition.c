#include "monsters/armor/actor_controller.h"
#include "monsters/armor/scene_combat_colliders.h"
#include "system/game_state/game_state.h"

extern struct GameState *gGameState;

/* tentative: 4-state charge attack with fade, collider arm and camera shake */

extern u32 gUnk_0824EE80[];
extern u32 gUnk_0824EE90[];
extern const struct ContactPosition gUnk_0824EEA0;
extern const struct ContactPosition gUnk_0824EEA8;
extern u32 gUnk_0824EEB0[];
void Armor_SetAnimationFromStart(struct SceneCombatActor *, const void *);
void Sound_PlaySongChecked(u32 a);
void Armor_StartFastWhiteFade(void *);
void Armor_StartPaletteTransition(void *);
void ActorPalette_Reset(void *);
void Collider_SetAttackPowerAndForce(void *, s32, s32);
void Camera_RequestShake(s32);
s32 Math_Atan2(s32, s32);

void ActorController_UpdateChargeAttackTransition(struct SceneCombatActor *actor)
{
    switch (actor->actionPhase) {
        case 0:
            Armor_SetAnimationFromStart(actor, &gUnk_0824EE80[((actor->heading + 0x60) >> 6) & 3]);
            actor->actionPhase++;
            actor->actionReady = 0;
            ActorController_SelectPairedColliders(actor);
            {
                struct ActorControllerMotion *motion = &actor->colliderGroup.motion;
                u32 mask = 1;
                motion->flags |= mask;
            }
            actor->flags &= ~0x20;
            Sound_PlaySongChecked(0xC1);
            break;
        case 1:
            if (actor->animationComplete != 0) {
                struct ContactCollider *c;
                Armor_SetAnimationFromStart(actor,
                                            &gUnk_0824EE90[((actor->heading + 0x60) >> 6) & 3]);
                actor->actionPhase++;
                actor->actionReady = 0;
                actor->actionCounter = 0;
                Armor_StartFastWhiteFade(actor);
                actor->shakeStrength = 20;
                actor->colliderGroup.colliders[0].flags &= ~4;
                c = &actor->colliderGroup.colliders[0];
                {
                    u32 mask = 0x40000;
                    c->reactionFlags |= mask;
                }
                Collider_SetAttackPowerAndForce(c, 0, 0x50);
                c->bounds = gUnk_0824EEA8;
                c->offset = gUnk_0824EEA0;
                actor->colliderGroup.colliders[0].direction =
                    Math_Atan2(gGameState->positionX - actor->navigation.referencePosition.x,
                               gGameState->positionZ - actor->navigation.referencePosition.z);
                Sound_PlaySongChecked(0x1BA);
            }
            break;
        case 2:
            if (actor->actionCounter > 7) {
                Armor_SetAnimationFromStart(actor,
                                            &gUnk_0824EEB0[((actor->heading + 0x60) >> 6) & 3]);
                actor->actionPhase++;
                actor->actionReady = 0;
                Armor_StartPaletteTransition(actor);
                actor->colliderGroup.colliders[0].flags |= 4;
            }
            break;
        case 3:
            if (actor->animationComplete != 0) {
                actor->actionPhase = 0xFF;
                actor->actionReady = 1;
                ActorPalette_Reset(actor);
            }
            break;
    }
    if (actor->shakeStrength != 0) {
        Camera_RequestShake(0x80);
        actor->shakeStrength--;
    }
}
