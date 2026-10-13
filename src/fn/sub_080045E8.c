#include "global.h"
#include "scene_object.h"
// MATCHING_CONCESSION: register-pinning
/* Tentative: apply a terrain-triggered resource cost and feedback. */
void sub_08215284(struct SceneAnimation *, u16);
void sub_0822B2F8(s32);
void AnimatedParticles_EmitBurst(struct SceneVec *, s32, s32, s32, s32, s32, s32, s32);
void Scene_ApplyTerrainInteraction(struct SceneCollider *contact, void *unused, struct SceneObject *actor)
{
    register struct SceneObject *incoming asm("r0") = actor;
    s32 applied;
    applied = 0;
    actor = incoming;
    if (actor->type == 0) {
        if (({
                u32 mask = 0x17;
                mask & contact->contactFlags;
            }) &&
            ({
                u32 mask = 0x10;
                mask & contact->contactFlags2;
            })) {
            actor->health -= 24;
            actor->damageTimer = 10;
            actor->animationTimer = 2;
            sub_08215284(&actor->animationData, actor->animation);
            applied = 1;
        }
    } else if (actor->type == 1) {
        if (({
                u32 mask = 4;
                mask & contact->contactFlags2;
            }) &&
            ({
                u32 mask = 0x80017;
                mask & contact->contactFlags;
            })) {
            actor->health -= 24;
            actor->damageTimer = 10;
            actor->animationTimer = 2;
            sub_08215284(&actor->animationData, actor->animation);
            if (!actor->cooldown) {
                actor->cooldown = 10;
                sub_0822B2F8(0x15C);
                AnimatedParticles_EmitBurst(&actor->body.position, 0, 0, 0x40, 12, 12, 4, 4);
            }
            applied = 1;
        }
    } else if (actor->type == 2) {
        if (({
                u32 mask = 1;
                mask & contact->contactFlags;
            })) {
            actor->health -= 24;
            actor->damageTimer = 10;
            actor->animationTimer = 2;
            sub_08215284(&actor->animationData, actor->animation);
            applied = 1;
        }
    }
    if (applied) {
        if ((u16)(actor->kind - 1) <= 1) {
            if (actor->health <= 0)
                actor->health = 1;
            else
                goto sound;
        }
        if (actor->health < 0)
            actor->health = 0;
        else {
        sound:
            sub_0822B2F8(0x13E);
        }
    }
}
