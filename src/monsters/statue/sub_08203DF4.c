#include "engine/video/display_sequence_sprite.h"
#include "monsters/statue.h"
#include "engine/video/render.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
/* Tentative species identity: applies damage, starts defeat effects, and advances animation. */
#include "engine/video/display_sequence_inline.h"
#include "engine/engine_pause.h"
#include "engine/word_flags.h"
#include "engine/math/monster_particle_random.h"
#include "player/player.h"
void Collider_SetPosition(void *collider, void *position, u32 value);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void Sound_PlaySong(u32 song);
void TerrainModifiers_UnregisterCell(void *);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
s32 Callback_InvokeActor(void *arg, void *function);
void SceneHud_SetTimedState(void *, int, int, int, int, int);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void Statue_UpdateDeath(void);

u32 Statue_UpdateDamageState(struct Statue *p)
{
    struct ContactCollider *collider;
    if (Engine_PauseBits(0x400) || Flags_MaskedWord(4, &gPlayerSlotStatus[0]))
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
    {
        if ((p->damage > 0) && (!p->dead)) {
            p->hp -= p->damage;
            p->hitFlags = p->colliderHitFlags;
            Sound_PlaySong(0x13e);
            if (p->damage > 0) {
                DamageNumberDisplay_Show(collider, p->damage, 0, &p->collider.position, &p->collider.offset, 1);
                SceneHud_SetTimedState(p->owner, p->hp, p->maxHp, p->hudStyle, p->collider.costFlags == 4,
                                       p->hudTime);
            }
            if (p->hp <= 0) {
                struct ContactPackedVector spread, velocity;
                struct ContactPosition center;
                const u16 *table;
                u32 sample;
                p->dead = 1;
                if (p->terrainEnabled)
                    TerrainModifiers_UnregisterCell(p->terrain);
                {
                    struct ContactPosition *cp = &center;
                    *cp = p->sprite.position;
                    cp->y += 0x12c;
                }
                velocity.x = -4;
                table = gRandomTable;
                sample = ParticleRandom_ReadDirectSample(table);
                velocity.y = sample & 7;
                velocity.z = -4;
                spread.x = 0x18;
                sample = ParticleRandom_ReadDirectSample(table);
                spread.y = (sample & 7) + 3;
                spread.z = 0x18;
                sample = ParticleRandom_ReadDirectSample(table);
                sub_08014634((sample & 3) + 4, 3, &center, &velocity, &spread, 15, 15);
                Anim_RestartSequence(&p->sequence, p->resource, 1, p->facing, 0);
                AnimData_SelectPaletteBank(&p->graphics, 0x1b7);
                p->graphics.paletteData = gPaletteBankData + 0x44a0;
                p->flash = 0;
                p->update = Statue_UpdateDeath;
                p->timer = 0;
                Sound_PlaySong(0x52a);
            }
            p->flash = 8;
            p->hit = 1;
            p->damage = 0;
        }
    }
    if ((p->flash > 0) && (p->dead == 0)) {
        AnimData_SelectPaletteBank(&p->graphics, 0x192);
        p->flash--;
    } else {
        AnimData_SelectPaletteBank(&p->graphics, 0x225);
    }
    if (p->collider.responseValue) {
        p->collider.responseValue--;
        if ((p->collider.responseValue >> 2) & 1)
            p->sprite.flags |= 1;
        else
            p->sprite.flags &= ~1;
    }
    DisplaySequence_StepSprite((void *)&p->sprite, &p->sequence);
    p->timer++;
    /* The callback has no explicit return-value assignment on this path in the ROM. */
}
