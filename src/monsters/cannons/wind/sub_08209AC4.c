#include "scene/collision/contact_actor.h"
#include "monsters/cannons/wind_cannon.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/ram.h"
#include "engine/engine_globals.h"
#include "engine/math/monster_particle_random.h"
s32 Mod(s32, s32);
s32 Div(s32, s32);
void WindCannon_Activate(struct FanCannon *);
void WindCannon_ToggleActiveState(struct FanCannon *);
void WindCannon_UpdateInactive(void);
void CombatActor_UpdateDefeatEffect(void);
s32 WindCannon_IsPathToPlayerClear(struct FanCannon *);
s32 Callback_InvokeActor(void *, void *);
void Collider_SetPosition(void *, void *, u32);
void Sound_PlaySong(u32);
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void SceneHud_SetTimedState(void *, s32, s32, s32, s32, s32);
void TerrainModifiers_UnregisterCell(void *);
void TerrainModifiers_RegisterCell(void *, s32, s32, s32, s32, s32);
void sub_08014634(s32, s32, void *, void *, void *, s32, s32);
void Explosion_CreateAt(void *, s32, s32, s32, s32, s32, s32, s32);
void Anim_RestartSequence(void *, void *, u32, u32, u32);
void AnimData_SelectPaletteBank(void *, s32);
struct ContactPosition *Player_GetEffectivePosition(s32);

static inline s32 Fan_Cell(struct ContactPosition *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline s32 Fan_Distance(struct FanCannon *p)
{
    struct ContactPosition *player = Player_GetEffectivePosition(0);
    s32 dx = player->x - p->sprite.position.x;
    s32 dz = player->z - p->sprite.position.z;
    return dx * dx + dz * dz;
}

s32 WindCannon_Update(void *manager, struct FanCannon *p, s32 slot)
{
    struct ContactCollider *collider;
    if (p->activated && p->dead == 0) {
        p->wind = p->maximumWind;
        WindCannon_Activate(p);
    }
    if (p->stopped) {
        if (p->dead)
            goto invoke;
        p->wind = 0;
        p->update = WindCannon_UpdateInactive;
    }
    if (p->dead == 0 && *(u16 *)&p->activated == 0) {
        if (p->schedule) {
            s16 *index = &p->scheduleIndex;
            if (p->age > p->times[*index]) {
                WindCannon_ToggleActiveState(p);
                p->age = 0;
                (*index)++;
                if (*index > p->scheduleCount - 1)
                    *index = 0;
            }
        } else if (Mod(p->age, p->period) == 0)
            WindCannon_ToggleActiveState(p);
    }
invoke:
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
    if (p->damage > 0) {
        if (p->dead != 0)
            goto tail;
        p->health -= p->damage;
        p->f158 = p->f15c;
        Sound_PlaySong(0x13e);
        if (p->damage > 0) {
            DamageNumberDisplay_Show(collider, p->damage, 0, &p->collider.position, &p->collider.offset,
                                     1);
            SceneHud_SetTimedState((void *)p->name, p->health, p->maximumHealth, p->hudA,
                                   p->collider.costFlags == 4, p->hudTime);
        }
        if (p->health <= 0) {
            struct ContactPackedVector spread, velocity;
            struct ContactPosition center;
            struct ContactPosition smoke;
            const u16 *table;
            p->dead = 1;
            if (p->terrain) {
                s32 cell;
                TerrainModifiers_UnregisterCell(p->terrainCell);
                cell = Fan_Cell(&p->sprite.position);
                TerrainModifiers_RegisterCell(p->terrainCell, cell, 0, p->sprite.position.y >> 8, 0, 1);
            }
            {
                struct ContactPosition *v = &center;
                *v = p->sprite.position;
                v->y += 0x80;
            }
            velocity.x = -4;
            table = (const u16 *)RANDOM_TABLE_ADDR;
            velocity.y = ParticleRandom_ReadDirectSample(table) & 7;
            velocity.z = -4;
            spread.x = 24;
            spread.y = (ParticleRandom_ReadDirectSample(table) & 7) + 3;
            spread.z = 24;
            sub_08014634((ParticleRandom_ReadDirectSample(table) & 3) + 4, 13, &center, &velocity, &spread, 15, 15);
            {
                struct ContactPosition *v = &smoke;
                *v = p->sprite.position;
                v->y += 0x80;
                Explosion_CreateAt(v, 16, 0, 0, 0, 60, 0, 0);
                v->x += 0x80;
                Explosion_CreateAt(v, 16, 0, 0, 0, 60, 0, 2);
                v->x -= 0x80;
                v->z += 0x80;
                Explosion_CreateAt(v, 16, 0, 0, 0, 60, 0, 2);
                v->z -= 0x80;
                v->x -= 0x80;
                Explosion_CreateAt(v, 16, 0, 0, 0, 60, 0, 5);
                v->x += 0x80;
                v->z -= 0x80;
                Explosion_CreateAt(v, 16, 0, 0, 0, 60, 0, 5);
            }
            Anim_RestartSequence(&p->sequence, p->resource, 0x16, (u8)p->direction, 0);
            AnimData_SelectPaletteBank(p->graphics, p->palette);
            p->flash = 0;
            p->update = CombatActor_UpdateDefeatEffect;
            p->age = 0;
            p->activated = 0;
            p->stopped = 0;
            Sound_PlaySong(0x52d);
        }
        p->flash = 8;
        p->reaction = 1;
        p->damage = 0;
    }
    if (p->dead == 0 && p->update != WindCannon_UpdateInactive && !p->stopped) {
        struct ContactPosition playerCopy;
        struct ContactPosition *pl;
        s32 direction;
        struct ContactPosition *src;
        src = Player_GetEffectivePosition(0);
        pl = &playerCopy;
        *pl = *src;
        direction = p->direction;
        if (direction == 0) {
            if (p->sprite.position.z - 0x80 < pl->z && p->sprite.position.z + 0x80 > pl->z &&
                p->sprite.position.x < pl->x && Fan_Distance(p) < p->range * p->range &&
                WindCannon_IsPathToPlayerClear(p)) {
                gPlayerBackgroundState = p->wind;
                if (!p->reverse)
                    gPlayerBackgroundMode = 0x80;
                else
                    gPlayerBackgroundMode = direction;
            }
        } else {
            if (p->sprite.position.x - 0x80 < pl->x && p->sprite.position.x + 0x80 > pl->x &&
                p->sprite.position.z < pl->z && Fan_Distance(p) < p->range * p->range &&
                WindCannon_IsPathToPlayerClear(p)) {
                gPlayerBackgroundState = p->wind;
                if (!p->reverse)
                    gPlayerBackgroundMode = 0xc0;
                else
                    gPlayerBackgroundMode = 0x40;
            }
        }
    }
tail:
    if (p->flash > 0 && p->dead == 0) {
        AnimData_SelectPaletteBank(p->graphics, 0x192);
        p->flash--;
    } else
        AnimData_SelectPaletteBank(p->graphics, p->palette);
    if (p->collider.responseValue) {
        p->collider.responseValue--;
        if ((p->collider.responseValue >> 2) & 1)
            p->sprite.flags |= 1;
        else
            p->sprite.flags &= ~1;
    }
    if (p->dead == 0) {
        if (p->wind <= 1)
            Anim_SetSequence(&p->sequence, p->resource, 0x14, (u8)p->direction, (u8)p->variant);
        else
            Anim_SetSequence(&p->sequence, p->resource, 0x15, (u8)p->direction, (u8)p->variant);
    }
    {
        s32 scale = 0x100 - Div(p->wind * 248, p->maximumWind);
        if (scale <= 7)
            scale = 8;
        Anim_SetDurationScale(&p->sequence, (u16)scale);
    }
    {
        u32 *sprite = &p->sprite.flags;
        PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite,
                               &p->sequence);
    }
    p->age++;
}
