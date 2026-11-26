#include "monsters/dragon_puppy.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/math/scene_vec.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "engine/engine_pause.h"
#include "engine/math/monster_particle_random.h"

#include "player/player.h"
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, u32, void *, void *, u32);
void SceneHud_SetTimedState(void *, s32, s32, s32, u32, s32);
void ImpactParticleGroup_SpawnBouncing(s32, s32, void *, void *, void *, s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void DragonPuppy_UpdateAttackWindup(void);

s32 DragonPuppy_UpdateCombat(struct DragonPuppy *p)
{
    struct ContactPosition position;
    struct ContactPackedVector velocity;
    struct ContactPackedVector size;
    struct ContactCollider *collider;
    if (Engine_PauseBits(0x400))
        goto paused;
    {
        u32 mask = 4;
        if (gPlayerSlotStatus[0] & mask) {
        paused:
            return 0;
        }
    }
    Callback_InvokeActor(p, p->update);
    p->sprite.flags &= ~1;
    if (p->defeated == 0) {
        struct ContactCollider *c = &p->collider;
        c->flags &= 0xfffb;
        Collider_SetPosition(c, &p->sprite.position, 0);
        collider = c;
    } else {
        struct ContactCollider *c = &p->collider;
        c->flags |= 4;
        collider = c;
    }
    if (p->damage > 0 && p->defeated == 0) {
        p->health -= p->damage;
        p->stateWord = p->savedStateWord;
        Sound_PlaySong(0x13e);
        if (p->damage > 0) {
            DamageNumberDisplay_Show(collider, p->damage, 0, &p->collider.position,
                                     &p->collider.offset, 1);
            SceneHud_SetTimedState(p->context, p->health, p->maxHealth, p->hudValue140,
                                   p->collider.costFlags == 4, p->hudValue148);
        }
        if (p->health <= 0) {
            const u16 *table;
            s32 count;
            p->defeated = 1;
            Sound_PlaySong(0x530);
            {
                struct ContactPosition *v = &position;
                *v = p->sprite.position;
                v->y += 200;
            }
            velocity.x = -4;
            table = gRandomTable;
            velocity.y = ParticleRandom_ReadDirectSample(table) & 3;
            velocity.z = -4;
            size.x = 24;
            size.y = (ParticleRandom_ReadDirectSample(table) & 3) + 1;
            size.z = 24;
            count = (ParticleRandom_ReadDirectSample(table) & 3) + 4;
            ImpactParticleGroup_SpawnBouncing(count, 0x16, &position, &velocity, &size, 15, 15);
            Anim_RestartSequence(&p->animation, p->deathAnimation, 5, p->deathVariant, 0);
            AnimData_SelectPaletteBank(p->palette, 0x1b9);
            p->flashTimer = 0;
            p->update = DragonPuppy_UpdateAttackWindup;
            p->ticks = 0;
        }
        p->flashTimer = 8;
        p->flashEnabled = 1;
        p->damage = 0;
    }
    if (p->flashTimer > 0) {
        AnimData_SelectPaletteBank(p->palette, 0x192);
        p->flashTimer--;
    } else
        AnimData_SelectPaletteBank(p->palette, 0x1b9);
    if (p->collider.responseValue) {
        p->collider.responseValue--;
        if ((p->collider.responseValue >> 2) & 1)
            p->sprite.flags |= 1;
        else
            p->sprite.flags &= ~1;
    }
    PlayerCue_StepSequence(&p->sprite, &p->animation);
    p->ticks++;
}
