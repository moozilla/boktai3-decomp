// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "ram.h"
struct FanVec {
    s16 x, y, z, pad;
};
struct FanSeq {
    const volatile u16 *commands;
    u8 flags, count, weight, duration;
    u16 index, unused, speed, elapsed;
};
struct FanCollider {
    u8 pad[6];
    u16 flags;
};
struct FanCannon {
    s32 age;
    void *hud;
    struct FanVec initial;
    u32 flags;
    u8 p14[0x18];
    struct FanVec position;
    u8 p34[8];
    u8 graphics[0x1c];
    struct FanCollider collider;
    u8 p60[4];
    u8 colliderPosition[8];
    u8 colliderExtra[0x2f];
    u8 colliderDirection;
    u16 blink;
    u8 p9e[0xf4 - 0x9e];
    void (*update)(void);
    struct FanSeq sequence;
    s32 range;
    s16 maximumWind, health, damage, reaction;
    s16 f114, dead, flash;
    s8 direction;
    u8 reverse;
    s16 period, palette, wind, variant;
    u8 activated, stopped;
    s16 hasBody;
    s16 maximumHealth, hudA, hudB, f12e, f130, f132, hudTime, initialDead;
    void *schedule;
    s32 f13c, terrain;
    void *resource;
    u8 terrainCell[8];
    s32 f150, f154, f158, f15c, f160, f164, f168;
    s16 scheduleCount, scheduleIndex;
    s16 times[8];
};
struct EffectPair {
    s32 lo : 16;
    u32 hi : 16;
};
struct EffectVector {
    struct EffectPair a, b;
};
struct FanTerrain {
    u8 pad[0x24];
    u16 rows[1];
};
extern struct FanTerrain *gTerrainMap;
extern u32 gTerrainWidth, gTerrainHeight, gRandomIndex;
s32 Mod(s32, s32);
s32 Div(s32, s32);
void FanCannon_Activate(struct FanCannon *);
void FanCannon_ToggleActiveState(struct FanCannon *);
void FanCannon_UpdateInactive(void);
void CombatActor_UpdateDefeatEffect(void);
s32 FanCannon_IsPathToPlayerClear(struct FanCannon *);
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
void Anim_SetSequence(void *, void *, u32, u32, u32);
void Anim_SetDurationScale(void *, u32);
void AnimData_SelectPaletteBank(void *, s32);
struct FanVec *Player_GetEffectivePosition(s32);

static inline u32 RandomFrom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 0x3ff;
    return table[gRandomIndex];
}
static inline s32 Fan_Cell(struct FanVec *v)
{
    s32 x = (s16)v->x >> 8, z = (s16)v->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline void Fan_Step(struct FanCannon *p, void *sprite, struct FanSeq *s)
{
    u32 flags, elapsed;
    u32 duration;
    register u32 mask asm("r5");
    register u32 savedDuration asm("r4");
    const volatile u16 *c = s->commands + s->index;
    *(u16 *)((u8 *)sprite + 0x10) = *c >> 6;
    {
        u32 input = s->flags, bit = 1;
        register u32 first asm("r2");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        if (first != (((*c & 0x30) >> 4) & bit))
            p->flags |= 4;
        else
            p->flags &= ~4;
    }
    {
        u32 input = s->flags, bit = 2;
        register u32 first asm("r2");
        asm("" : "+r"(bit));
        first = bit;
        first &= input;
        asm("" : "+r"(first));
        first = (u8)first;
        if (first != (((*c & 0x30) >> 4) & bit))
            *(u32 *)sprite |= 8;
        else
            *(u32 *)sprite &= ~8;
    }
    elapsed = s->elapsed + 1;
    s->elapsed = elapsed;
    {
        register u32 temp asm("r2") = 0xffff;
        asm("" : "+r"(temp));
        mask = temp;
    }
    duration = s->duration;
    elapsed = (u16)elapsed;
    savedDuration = duration;
    asm("" : "+r"(savedDuration) : "r"(elapsed));
    if ((u16)elapsed >= duration) {
        s->elapsed = 0;
        flags = s->flags;
        if (flags & 4) {
            u32 i = s->index;
            if (!i) {
                {
                    u32 hold = 8;
                    hold &= flags;
                    if (hold)
                        s->elapsed = savedDuration;
                    else
                        s->index = s->count - 1;
                }
            } else
                s->index = i - 1;
        } else {
            u32 i = s->index + 1, n;
            s->index = i;
            n = s->count;
            if ((i & mask) >= n) {
                {
                    u32 hold = 8;
                    hold &= flags;
                    if ((u8)hold) {
                        s->elapsed = duration;
                        s->index = n - 1;
                    } else
                        s->index = 0;
                }
            }
        }
        c = s->commands + s->index;
        s->weight = *c & 15;
        {
            register s32 w asm("r2") = s->weight;
            s32 speed = s->speed;
            speed = (speed * w) >> 6;
            s->duration = speed;
            if (!(u8)speed)
                s->duration = 1;
        }
    }
}
static inline s32 Fan_Distance(struct FanCannon *p)
{
    struct FanVec *player = Player_GetEffectivePosition(0);
    s32 dx = player->x - p->position.x;
    s32 dz = player->z - p->position.z;
    return dx * dx + dz * dz;
}

s32 FanCannon_Update(void *manager, struct FanCannon *p, s32 slot)
{
    struct FanCollider *collider;
    if (p->activated && p->dead == 0) {
        p->wind = p->maximumWind;
        FanCannon_Activate(p);
    }
    if (p->stopped) {
        if (p->dead)
            goto invoke;
        p->wind = 0;
        p->update = FanCannon_UpdateInactive;
    }
    if (p->dead == 0 && *(u16 *)&p->activated == 0) {
        if (p->schedule) {
            s16 *index = &p->scheduleIndex;
            if (p->age > p->times[*index]) {
                FanCannon_ToggleActiveState(p);
                p->age = 0;
                (*index)++;
                if (*index > p->scheduleCount - 1)
                    *index = 0;
            }
        } else if (Mod(p->age, p->period) == 0)
            FanCannon_ToggleActiveState(p);
    }
invoke:
    Callback_InvokeActor(p, p->update);
    p->flags &= ~1;
    if (p->dead == 0) {
        struct FanCollider *c = &p->collider;
        c->flags &= ~4;
        Collider_SetPosition(c, &p->position, 0);
        collider = c;
    } else {
        struct FanCollider *c = &p->collider;
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
            DamageNumberDisplay_Show(collider, p->damage, 0, p->colliderPosition, p->colliderExtra,
                                     1);
            SceneHud_SetTimedState(p->hud, p->health, p->maximumHealth, p->hudA,
                                   p->colliderDirection == 4, p->hudTime);
        }
        if (p->health <= 0) {
            struct EffectVector spread, velocity;
            struct FanVec center;
            struct FanVec smoke;
            const u16 *table;
            p->dead = 1;
            if (p->terrain) {
                s32 cell;
                TerrainModifiers_UnregisterCell(p->terrainCell);
                cell = Fan_Cell(&p->position);
                TerrainModifiers_RegisterCell(p->terrainCell, cell, 0, p->position.y >> 8, 0, 1);
            }
            {
                struct FanVec *v = &center;
                *v = p->position;
                v->y += 0x80;
            }
            velocity.a.lo = -4;
            table = (const u16 *)RANDOM_TABLE_ADDR;
            velocity.a.hi = RandomFrom(table) & 7;
            velocity.b.lo = -4;
            spread.a.lo = 24;
            spread.a.hi = (RandomFrom(table) & 7) + 3;
            spread.b.lo = 24;
            sub_08014634((RandomFrom(table) & 3) + 4, 13, &center, &velocity, &spread, 15, 15);
            {
                struct FanVec *v = &smoke;
                *v = p->position;
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
        {
            s16 *flash = &p->flash;
            s32 zero = 0;
            *flash = 8;
            p->reaction = 1;
            p->damage = zero;
        }
    }
    if (p->dead == 0 && p->update != FanCannon_UpdateInactive && !p->stopped) {
        struct FanVec playerCopy;
        struct FanVec *pl;
        s32 direction;
        struct FanVec *src;
        src = Player_GetEffectivePosition(0);
        pl = &playerCopy;
        *pl = *src;
        direction = p->direction;
        if (direction == 0) {
            if (p->position.z - 0x80 < pl->z && p->position.z + 0x80 > pl->z &&
                p->position.x < pl->x && Fan_Distance(p) < p->range * p->range &&
                FanCannon_IsPathToPlayerClear(p)) {
                gPlayerBackgroundState = p->wind;
                if (!p->reverse)
                    gPlayerBackgroundMode = 0x80;
                else
                    gPlayerBackgroundMode = direction;
            }
        } else {
            if (p->position.x - 0x80 < pl->x && p->position.x + 0x80 > pl->x &&
                p->position.z < pl->z && Fan_Distance(p) < p->range * p->range &&
                FanCannon_IsPathToPlayerClear(p)) {
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
    if (p->blink) {
        p->blink--;
        if ((p->blink >> 2) & 1)
            p->flags |= 1;
        else
            p->flags &= ~1;
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
        u32 *sprite = &p->flags;
        Fan_Step(p, sprite, &p->sequence);
    }
    p->age++;
}
