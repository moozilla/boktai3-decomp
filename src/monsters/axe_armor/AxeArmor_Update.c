#include "monsters/axe_armor/axe_armor.h"
#include "engine/video/display_animation.h"
#include "engine/math/scene_vec.h"
#include "engine/video/render.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/engine_pause.h"
#include "engine/word_flags.h"
#include "engine/math/monster_particle_random.h"
#include "scene/collision/packed_contact_halfwords.h"
extern u32 gUnk_02000560;
s32 Callback_InvokeActor(void *, void *);
void Collider_SetPosition(void *, void *, u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void TerrainModifiers_UnregisterCell(void *);
void ImpactParticleGroup_SpawnBouncing(s32, s32, void *, void *, void *, s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, s32);
s32 AxeArmor_UpdateDeath(struct AxeArmor *);
struct EffectVector {
    struct PackedContactHalfwords a, b;
};
s32 AxeArmor_Update(struct AxeArmor *p)
{
    struct ContactCollider *collider;
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gUnk_02000560))
        return 0;
    Callback_InvokeActor(p, p->update);
    p->sprite.flags &= ~1;
    if (p->dead == 0) {
        struct ContactCollider *c = &p->collider;
        c->flags &= ~4;
        Collider_SetPosition(c, &p->sprite.position, 0);
        collider = c;
    } else {
        struct ContactCollider *c = &p->collider;
        c->flags |= 4;
        collider = c;
    }
    if (p->pendingDamage > 0 && p->dead == 0) {
        p->health -= p->pendingDamage;
        Sound_PlaySong(0x13e);
        if (p->pendingDamage > 0) {
            p->displayedDamageType = p->damageType;
            DamageNumberDisplay_Show(collider, p->pendingDamage, 0, &p->collider.position,
                                     &p->collider.offset, 1);
            SceneHud_SetTimedState(p->parent, p->health, p->maxHealth, p->hudStyle,
                                   p->collider.costFlags == 4, p->hudTime);
        }
        if (p->health <= 0) {
            struct EffectVector spread, velocity;
            struct SceneVec center;
            const u16 *table;
            s16 *position;
            s16 *z;
            p->dead = 1;
            if (p->terrainEnabled)
                TerrainModifiers_UnregisterCell(p->terrain);
            {
                struct SceneVec *v = &center;
                position = &p->sprite.position.x;
                *v = *(struct SceneVec *)position;
                v->y += 200;
            }
            velocity.a.lo = -4;
            table = (const u16 *)RANDOM_TABLE_ADDR;
            velocity.a.hi = ParticleRandom_ReadDirectSample(table) & 7;
            velocity.b.lo = -4;
            spread.a.lo = 24;
            spread.a.hi = (ParticleRandom_ReadDirectSample(table) & 7) + 3;
            spread.b.lo = 24;
            ImpactParticleGroup_SpawnBouncing((ParticleRandom_ReadDirectSample(table) & 3) + 4, 13, &center, &velocity,
                         &spread, 15, 15);
            Anim_RestartSequence(&p->sequence, p->resource, 4, 0, 0);
            *position = p->initialPosition.x + 0x40;
            {
                u16 v = p->initialPosition.z + 0x40;
                z = &p->sprite.position.z;
                *z = v;
            }
            AnimData_SelectPaletteBank(&p->graphics, 0x1b7);
            p->graphics.paletteData = gPaletteBankData + 0x44a0;
            p->flashTimer = 0;
            p->update = AxeArmor_UpdateDeath;
            p->timer = 0;
            *position -= 0x60;
            *z -= 0x60;
            Sound_PlaySong(0x52b);
        }
        {
            s16 *flash = &p->flashTimer;
            s32 zero = 0;
            *flash = 8;
            p->hitFlash = 1;
            p->pendingDamage = zero;
        }
    }
    if (p->flashTimer > 0 && p->dead == 0) {
        AnimData_SelectPaletteBank(&p->graphics, 0x192);
        p->flashTimer--;
    } else
        AnimData_SelectPaletteBank(&p->graphics, 0x227);
    if (p->collider.responseValue) {
        p->collider.responseValue--;
        if ((p->collider.responseValue >> 2) & 1)
            p->sprite.flags |= 1;
        else
            p->sprite.flags &= ~1;
    }
    PlayerCue_StepSequence((struct ContactSwarmSprite *)&p->sprite,
                           &p->sequence);
    p->timer++;
}
