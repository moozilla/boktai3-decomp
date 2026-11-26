#include "scene/collision/contact_actor.h"
#include "monsters/cannons/plasma_cannon.h"
#include "ui/hud/damage_number_display.h"
#include "ui/pause/scene_hud_display.h"
#include "engine/video/display_sequence_inline.h"
#include "engine/video/display_animation.h"
#include "engine/math/scene_vec.h"
#include "engine/video/render.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/engine_pause.h"
#include "engine/word_flags.h"
#include "engine/math/monster_particle_random.h"
// confidence=low: handles collision damage, defeat effects, palette flashing and animation.
#include "player/player.h"
s32 Callback_InvokeActor(void *, void *);
void Collider_SetPosition(void *, void *, u32);
void TerrainModifiers_UnregisterCell(void *);
void ImpactParticleGroup_SpawnBouncing(s32, s32, void *, void *, void *, s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, s32);
void PlasmaCannon_UpdateDefeatEffect(u8 *);
void Explosion_CreateAt(void *, s32, s32, s32, s32, s32, s32, s32);
static inline u16 PlasmaCannon_RandomBurstCount(const u16 *table)
{
    return (ParticleRandom_ReadDirectSample(table) & 3) + 4;
}
static inline u32 PlasmaCannon_StepSequence(struct DisplaySequenceSprite *owner,
                                            struct DisplaySequenceState *state)
{
    const u16 *command;
    u32 ended;
    {
        u32 offset = DisplaySequence_GetIndex(state) << 1;
        const u16 *table;
        table = state->commands;
        command = (const u16 *)((u32)table + offset);
    }
    DisplaySequence_ApplyFrame((struct SceneActorControlMotionDrawable *)owner, state, command);
    if (++state->elapsed >= state->duration) {
        state->elapsed = 0;
        if (state->flags & 4) {
            if (state->index == 0) {
                if (state->flags & 8)
                    state->elapsed = state->duration;
                else
                    state->index = state->frameCount - 1;
                ended = 1;
            } else {
                state->index--;
                ended = 0;
            }
        } else {
            if (++state->index >= state->frameCount) {
                if (state->flags & 8) {
                    state->elapsed = state->duration;
                    state->index = state->frameCount - 1;
                } else {
                    state->index = 0;
                }
                ended = 1;
            } else {
                ended = 0;
            }
        }
        command = state->commands + state->index;
        state->frameWeight = DisplaySequence_MaskCommand(DisplaySequence_ReadCommand(command), 15);
        state->duration = (state->frameWeight * (s32)state->speed) >> 6;
        if (state->duration == 0)
            state->duration = 1;
    } else {
        ended = 0;
    }
    return ended;
}

s32 PlasmaCannon_UpdateDamageState(struct OrangeSpineActor *p)
{
    struct ContactCollider *collider;
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gPlayerSlotStatus[0]))
        return 0;
    Callback_InvokeActor(p, p->update);
    p->sprite.display.flags &= ~1;
    if (p->defeated == 0) {
        struct ContactCollider *c = &p->collider;
        c->flags &= ~4;
        Collider_SetPosition(c, &p->sprite.coordinates.position, 0);
        collider = c;
    } else {
        struct ContactCollider *c = &p->collider;
        c->flags |= 4;
        collider = c;
    }
    if (p->damage > 0 && p->defeated == 0) {
        p->health -= p->damage;
        p->reactionFlags = p->pendingReaction;
        Sound_PlaySong(0x13e);
        if (p->damage > 0) {
            DamageNumberDisplay_Show((struct DamageNumberSource *)collider, p->damage, 0,
                                     &p->collider.position, &p->collider.offset, 1);
            SceneHud_SetTimedState((u32)p->hud, p->health, p->maxHealth, p->hudStyle,
                                   p->collider.costFlags == 4, p->hudTime);
        }
        if (p->health <= 0) {
            struct SceneVec center, smoke;
            struct ContactPackedVector velocity, spread;
            const u16 *table;
            p->defeated = 1;
            p->collider.responseValue = 0;
            {
                struct SceneVec *centerPosition = &center;
                *centerPosition = p->sprite.coordinates.position;
                centerPosition->y += 128;
            }
            velocity.x = -4;
            table = gRandomTable;
            velocity.y = ParticleRandom_ReadDirectSample(table) & 7;
            velocity.z = -4;
            spread.x = 24;
            spread.y = (ParticleRandom_ReadDirectSample(table) & 7) + 3;
            spread.z = 24;
            ImpactParticleGroup_SpawnBouncing(PlasmaCannon_RandomBurstCount(table), 13, &center,
                                              &velocity, &spread, 15, 15);
            {
                struct SceneVec *smokePosition = &smoke;
                *smokePosition = p->sprite.coordinates.position;
                smokePosition->y += 128;
            }
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 0);
            smoke.x += 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 2);
            smoke.x -= 128;
            smoke.z += 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 2);
            smoke.z -= 128;
            smoke.x -= 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 5);
            smoke.x += 128;
            smoke.z -= 128;
            Explosion_CreateAt(&smoke, 16, 0, 0, 0, 60, 0, 5);
            Anim_RestartSequence(&p->sequence, p->resource, 16, p->facing, 0);
            AnimData_SelectPaletteBank(p->graphics, 0x1c1);
            p->flashTimer = 0;
            p->update = PlasmaCannon_UpdateDefeatEffect;
            p->age = 0;
            Sound_PlaySong(0x52d);
        }
        p->flashTimer = 8;
        p->hitReaction = 1;
        p->damage = 0;
    }
    if (p->flashTimer > 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->flashTimer--;
    } else
        AnimData_SelectPaletteBank(p->graphics, 0x1c1);
    if (p->collider.responseValue) {
        p->collider.responseValue--;
        if ((p->collider.responseValue >> 2) & 1)
            p->sprite.display.flags |= 1;
        else
            p->sprite.display.flags &= ~1;
    }
    PlasmaCannon_StepSequence(&p->sprite.display, &p->sequence);
    p->age++;
}
