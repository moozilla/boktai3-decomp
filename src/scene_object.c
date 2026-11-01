#include "global.h"
#include "scene_object.h"
#include "ram.h"
#include "monster_particle_random.h"

static inline void scatter(u16 *coordinate, const u16 *sample)
{
    s32 value = *coordinate - 128;
    u32 noise = (u8)*sample;
    *coordinate = value + noise;
}

void Scene_SpawnEdgeParticles(struct SceneVec *position, s32 offset)
{
    struct SceneExtent size;
    struct SceneVec point;
    u16 *p;
    s32 i;
    size.x = 32;
    size.y = 0;
    size.z = 32;
    for (i = 0; i < 10; i++) {
        const u16 *table = gRandomTable;
        p = (u16 *)&point;
        gRandomIndex = (gRandomIndex + 1) & 1023;
        point = *position;
        if (!(i & 1)) {
            p[0] += offset + (ParticleRandom_ReadDirectSample(table) & 15);
            gRandomIndex = (gRandomIndex + 1) & 1023;
            scatter(&p[2], &table[gRandomIndex]);
        } else {
            gRandomIndex = (gRandomIndex + 1) & 1023;
            scatter(&p[0], &table[gRandomIndex]);
            p[2] += offset + (ParticleRandom_ReadDirectSample(table) & 15);
        }
        table = 0;
        SceneParticleBurst_CreateWithPause(&point, (s32)table, &size, 1, 3, 4);
    }
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);

u8 Scene_CanEnterNeighborCell(struct SceneObject *object, s32 index, s32 direction)
{
    u8 *reference = &object->cellRef.cell.height;
    s32 delta = 0, neighbor;
    struct SceneCell *cell;
    u8 *dynamic;
    switch (direction) {
        case 0:
            delta = -((s16 *)gTerrainMap->cells)[2];
            break;
        case 1:
            delta = 1;
            break;
        case 2:
            delta = ((s16 *)gTerrainMap->cells)[2];
            break;
        case 3:
            delta = -1;
            break;
    }
    neighbor = index + delta;
    dynamic = TerrainModifiers_FindCellOverride(neighbor, 1);
    if (dynamic)
        cell = (struct SceneCell *)(dynamic + 4);
    else
        cell = (struct SceneCell *)(gTerrainMap->cells + 12 + neighbor * 4);
    if (cell) {
        if (!(cell->height >> 4)) {
            if ((u32)(cell->height & 15) < (u32)(*reference & 15))
                goto fail;
            if (cell->flags & 8)
                goto fail;
            goto success;
        } else if ((u32)(cell->height & 15) >= (u32)(*reference & 15))
            goto success;
    }
fail:
    return 0;
success:
    return 1;
}

void Scene_PlayNeighborStepSound(u32 type)
{
    if (type == 5) {
        Sound_PlaySong(0x151);
    } else {
        Sound_PlaySong(0x151);
    }
}

u8 Scene_CheckNeighborHeight(struct SceneObject *object, s32 index, s32 direction)
{
    u8 *reference = &object->cellRef.cell.height;
    s32 delta = 0, neighbor;
    struct SceneCell *cell;
    u8 *dynamic;
    switch (direction) {
        case 0:
            delta = -((s16 *)gTerrainMap->cells)[2];
            break;
        case 1:
            delta = 1;
            break;
        case 2:
            delta = ((s16 *)gTerrainMap->cells)[2];
            break;
        case 3:
            delta = -1;
            break;
    }
    neighbor = index + delta;
    dynamic = TerrainModifiers_FindCellOverride(neighbor, 1);
    if (dynamic)
        cell = (struct SceneCell *)(dynamic + 4);
    else
        cell = (struct SceneCell *)(gTerrainMap->cells + 12 + neighbor * 4);
    if (cell) {
        if ((u32)(cell->height & 15) >= (u32)(*reference & 15))
            return 1;
        if (object->type == 5)
            Scene_PlayNeighborStepSound(object->type);
    }
    return 0;
}

/* Count how many of the four neighbors accept the object; more than two means it is boxed in. */

u8 Scene_IsObjectSurrounded(struct SceneObject *object)
{
    s32 c = 0;
    s32 i = 0;
    do {
        if (Scene_CanEnterNeighborCell(object, object->cellRef.index, i))
            c++;
        i++;
    } while (i <= 3);
    if (c > 2)
        return 1;
    return 0;
}

void Scene_PlayMovementEffect(struct SceneObject *object)
{
    switch (object->type) {
        case 0:
            Sound_PlaySong(0x143);
            break;
        case 1:
            Sound_PlaySong(0x143);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            Sound_PlaySong(0x143);
            break;
        default:
            Sound_PlaySong(0x143);
            break;
    }
    if (Scene_IsObjectSurrounded(object))
        AnimatedParticles_EmitDirectional(4, 1, 1, 0x40, 0, 255, 2, 4, 6, 6, &object->center);
    else
        Scene_SpawnEdgeParticles(&object->body.position, 0x90);
}

void Scene_PlayNeighborMovementEffect(struct SceneObject *object)
{
    struct SceneVec position;
    s32 angle;
    if (Scene_CheckNeighborHeight(object, object->cellRef.index, object->direction)) {
        position = object->center;
        angle = 0;
        switch (object->direction) {
            case 0:
                position.z -= 128;
                angle = 0xB0;
                break;
            case 1:
                position.x += 128;
                angle = -16;
                break;
            case 2:
                position.z += 128;
                angle = 0x30;
                break;
            case 3:
                position.x -= 128;
                angle = 0x70;
                break;
        }
        AnimatedParticles_EmitDirectional(2, 1, 1, 0x40, angle, 31, 2, 4, 6, 6, &position);
        Scene_PlayNeighborStepSound(object->type);
    }
}

/* Play the sound for pushing an object of this type. */

void SceneObjects_PlayPushSound(s32 type)
{
    switch (type) {
        case 0:
        case 2:
        case 3:
        case 4:
        case 5:
            Sound_PlaySong(0x142);
            break;
        case 1:
            Sound_PlaySong(0x137);
            break;
        default:
            Sound_PlaySong(0x142);
            break;
    }
}

void AnimatedParticles_EmitBurst(struct SceneVec *, s32, s32, s32, s32, s32, s32, s32);

void Scene_ApplyTerrainInteraction(struct SceneCollider *contact, void *unused,
                                   struct SceneObject *object)
{
    s32 applied;
    struct SceneObject *actor;
    applied = 0;
    actor = object;
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
            AnimData_SelectPaletteBank(&actor->animationData, actor->animation);
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
            AnimData_SelectPaletteBank(&actor->animationData, actor->animation);
            if (!actor->cooldown) {
                actor->cooldown = 10;
                Sound_PlaySong(0x15C);
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
            AnimData_SelectPaletteBank(&actor->animationData, actor->animation);
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
            Sound_PlaySong(0x13E);
        }
    }
}

void SceneObjects_NoOpUpdate(void)
{
}

/* Start falling for objects resting above another object in the same cell. */

typedef void (*UpdateFn)(struct SceneObject *);

static inline UpdateFn fallCallback(void)
{
    return Scene_UpdateFallingObject;
}

void Scene_DropObjectsAbove(struct SceneObject *object)
{
    struct SceneObjectManager *manager = gSceneObjectManager;
    struct SceneObject *p = manager->objects;
    s32 i;
    for (i = 0; i < manager->count; i++, p++) {
        if (!p->active)
            continue;
        if (p == object)
            continue;
        {
            u8 *height = &p->cellRef.cell.height;
            u8 *reference = &object->cellRef.cell.height;
            if (p->cellRef.index != object->cellRef.index)
                continue;
            if (p->kind != 0)
                continue;
            if ((u32)(*height & 15) <= (u32)(*reference & 15))
                continue;
            p->update = fallCallback();
            p->timer = 0;
            p->entered = 1;
            p->kind = 2;
            p->dx = 0;
            p->dz = 0;
            p->direction = 0;
            p->targetY = p->body.position.y;
            if (p->targetY > 255)
                p->targetY = p->body.position.y - 256;
            else
                p->targetY = 0;
        }
    }
}

void TerrainModifiers_UnregisterCell(void *);

void TerrainOverlay_QueueRemoveActor(void *);

void SceneBody_Unregister(void *);

/* Release everything an object holds (draw, cell reference, collider, body) and free its slot. */

u32 SceneObjects_DestroyObject(struct SceneObject *object)
{
    Sprite_Remove(&object->draw);
    TerrainModifiers_UnregisterCell(&object->cellRef);
    Collider_Remove(&object->collider);
    if ((object->flags & 2) == 0)
        TerrainOverlay_QueueRemoveActor(&object->body);
    SceneBody_Unregister(&object->body);
    return object->active = 0;
}

/* Free every active object. */

s32 SceneObjects_DestroyAllObjects(void)
{
    struct SceneObjectManager *manager = gSceneObjectManager;
    struct SceneObject *object;
    s32 i;
    if (manager == 0)
        return -1;
    object = manager->objects;
    for (i = 0; i < manager->count; i++) {
        if (object->active != 0)
            SceneObjects_DestroyObject(object);
        object = object + 1;
    }
    return 0;
}

/* Make every idle type-1 object start disappearing. */

s32 Scene_ActivateTypeOneObjects(void)
{
    struct SceneObjectManager *manager = gSceneObjectManager;
    struct SceneObject *p;
    s32 i;
    if (!manager)
        return -1;
    p = manager->objects;
    for (i = 0; i < manager->count; p++, i++) {
        if (!p->active)
            continue;
        if (p->type != 1)
            continue;
        p->health = 0;
        if (!p->kind) {
            p->update = Scene_UpdateObjectDisappearance;
            p->timer = 0;
            p->entered = 1;
            p->kind = 3;
        }
    }
    return 0;
}

// MATCHING_CONCESSION: register-pinning
s32 Scene_StopSelectedObject(void)
{
    struct SceneObjectManager *manager = gSceneObjectManager;
    register u32 id asm("r6");
    struct SceneObject *p;
    register s32 i asm("r5");
    register s32 count asm("r4");
    if (!manager || !(id = Script_GetKeywordValue(0x6E, 0)))
        return -1;
    p = manager->objects;
    i = 0;
    count = manager->count;
    for (; i < count; p++, i++) {
        if (!p->active)
            continue;
        if (p->type == 1)
            continue;
        if (p->kind)
            continue;
        if (p->body.id == id) {
            p->health = 0;
            p->mode = 3;
            p->update = 0;
            return 1;
        }
    }
    return 0;
}

/* Free every active object whose script id matches the selected one. */

s32 SceneObjects_DestroyObjectsByScriptId(void)
{
    struct SceneObjectManager *manager = gSceneObjectManager;
    u32 id;
    struct SceneObject *object;
    s32 i;
    if (manager == 0)
        return -1;
    id = Script_GetKeywordValue(0x6e, 0);
    object = manager->objects;
    for (i = 0; i < manager->count; object++, i++) {
        if (object->active != 0 && object->body.id == id)
            SceneObjects_DestroyObject(object);
    }
    return 0;
}

void sub_08014CA4(u32, u32, struct SceneVec *, u32, u32, u32, u32, u32, u32, u32, u32, u32);

void sub_08014634(u32, u32, struct SceneExtent *, struct SceneExtent *, struct SceneExtent *, u32,
                  u32);

void Scene_PlayObjectBreakEffect(struct SceneObject *p)
{
    struct SceneExtent a;
    struct SceneExtent b;
    struct SceneExtent c;
    u32 d;
    struct SceneVec *q;
    Sound_PlaySong(0x15B);
    q = &p->body.position;
    d = 0x1e;
    sub_08014CA4(3, 2, q, 0x3c, d, 0x10, 8, 8, 0, 0x100, 0x18, 0x10);
    sub_08014CA4(8, 6, q, 0x3c, d, 0x16, 8, 8, 0, 0x100, 0x18, 0x10);
    a.x = p->body.position.x;
    a.y = p->body.position.y + 0x80;
    a.z = p->body.position.z;
    b.x = 0xFFF1;
    b.y = 8;
    b.z = 0xFFF1;
    c.x = d;
    c.y = 0x16;
    c.z = d;
    sub_08014634(8, 3, &a, &b, &c, 0x3c, 0x3c);
}

void Scene_PlayObjectSplashEffect(struct SceneObject *p)
{
    struct SceneExtent a;
    struct SceneExtent b;
    struct SceneExtent c;
    u32 d;
    struct SceneVec *q;
    Sound_PlaySong(0x135);
    q = &p->body.position;
    d = 0x1e;
    sub_08014CA4(4, 0, q, 0x3c, d, 0x14, 8, 8, 0, 0x100, 0x18, 0x10);
    a.x = p->body.position.x;
    a.y = p->body.position.y + 0x80;
    a.z = p->body.position.z;
    b.x = 0xFFF1;
    b.y = 8;
    b.z = 0xFFF1;
    c.x = d;
    c.y = 0x16;
    c.z = d;
    sub_08014634(8, 2, &a, &b, &c, 0x3c, 0x3c);
}

void Scene_PlayObjectImpactEffect(struct SceneObject *object)
{
    struct SceneVec *p;
    Sound_PlaySong(0x150);
    p = &object->body.position;
    sub_08014CA4(3, 1, p, 0x3c, 0x1e, 0x10, 8, 8, 0, 0x100, 0x18, 0x10);
    sub_08014CA4(8, 5, p, 0x3c, 0x1e, 0x16, 8, 8, 0, 0x100, 0x18, 0x10);
}

/* Local view of a SceneVec as 32-bit words; needed to match the halfword read-modify-write. */

struct Vec {
    union {
        u32 word;
        struct {
            u16 x, y;
        } half;
    } xy;
    union {
        u32 word;
        struct {
            u16 z, pad;
        } half;
    } zp;
};

static inline u32 dustScatter(const u8 *random, const u16 *coordinate)
{
    s32 value = *coordinate - 128;
    return value + *random;
}

static inline u32 frontEdge(u32 random, u32 coordinate)
{
    s32 value = coordinate + 120;
    return value + (random & 15);
}

static inline u32 backEdge(u32 random, u32 coordinate)
{
    s32 value = coordinate - 120;
    return value - (random & 15);
}

/* Consume two jitter samples per particle; publish the index before emission. */

#define FIRST()                                                                                    \
    (index = (*cursor + 1) & ({                                                                    \
                 register u32 m asm("r1") = mask;                                                  \
                 asm("" : "+r"(m));                                                                \
                 m;                                                                                \
             }),                                                                                   \
     (const u16 *)(index * 2 + (u32)table))
#define LAST()                                                                                     \
    (index = (index + 1) & mask, *({                                                               \
         register u32 *p asm("r1") = cursor;                                                       \
         p;                                                                                        \
     }) = index,                                                                                   \
     index *= 2, index += (u32)table, (const u16 *)index)
void Scene_SpawnDirectionalDust(struct SceneObject *input)
{
    register struct SceneObject *actor asm("r10") = input;
    struct SceneExtent size;
    struct Vec point;
    struct Vec *particle;
    s32 i;
    register const u16 *table asm("r4");
    register u32 *cursor asm("r9");
    u32 mask;
    size.x = 32;
    size.y = 0;
    size.z = 32;
    particle = &point;
    table = gRandomTable;
    cursor = &gRandomIndex;
    i = ({
        register s32 count asm("r1");
        asm("" : "=r"(count) : "0"(2));
        count;
    });
    mask = 1023;
    do {
        register u32 index asm("r2");
        struct SceneObject *local = actor;

        const u16 *random;

        point = *(struct Vec *)&local->body.position;
        switch (local->direction) {
            case 0:
                random = FIRST();
                {
                    u32 value = dustScatter((u8 *)random, &particle->xy.half.x);
                    u32 low = (u16)value;
                    point.xy.word = (point.xy.word & 0xFFFF0000) | low;
                }
                {
                    const u16 *second = LAST();
                    {
                        u32 value = frontEdge(*second, point.zp.half.z);
                        u32 low = (u16)value;
                        point.zp.word = (point.zp.word & 0xFFFF0000) | low;
                    }
                }
                break;
            case 1:
                random = FIRST();
                {
                    u32 value = backEdge(*random, particle->xy.half.x);
                    u32 low = (u16)value;
                    point.xy.word = (point.xy.word & 0xFFFF0000) | low;
                }
                {
                    const u16 *second = LAST();
                    {
                        u32 value = dustScatter((u8 *)second, &point.zp.half.z);
                        u32 low = (u16)value;
                        point.zp.word = (point.zp.word & 0xFFFF0000) | low;
                    }
                }
                break;
            case 2:
                random = FIRST();
                {
                    register u32 value asm("r1") = dustScatter((u8 *)random, &particle->xy.half.x);
                    u32 low = (u16)value;
                    point.xy.word = (point.xy.word & 0xFFFF0000) | low;
                }
                {
                    const u16 *second = LAST();
                    {
                        u32 value = backEdge(*second, point.zp.half.z);
                        u32 low = (u16)value;
                        point.zp.word = (point.zp.word & 0xFFFF0000) | low;
                    }
                }
                break;
            case 3:
                random = FIRST();
                {
                    register u32 value asm("r1") = frontEdge(*random, particle->xy.half.x);
                    u32 low = (u16)value;
                    point.xy.word = (point.xy.word & 0xFFFF0000) | low;
                }
                {
                    const u16 *second = LAST();
                    {
                        register u32 value asm("r1") = dustScatter((u8 *)second, &point.zp.half.z);
                        u32 low = (u16)value;
                        point.zp.word = (point.zp.word & 0xFFFF0000) | low;
                    }
                }
                break;
        }
        SceneParticleBurst_CreateWithPause((struct SceneVec *)particle, 0, &size, 1, 2, 4);
        i = ({
            register s32 count asm("r2") = i;
            asm("" : : "r"(count));
            count - 1;
        });
    } while (i >= 0);
}

// MATCHING_CONCESSION: register-pinning
s32 Scene_TryStepDown(struct SceneObject *input, s32 idx, s32 dir)
{
    struct SceneObject *actor = input;
    register s32 index asm("r4") = idx;
    s32 direction = dir;
    struct SceneCell *reference = &actor->cellRef.cell;
    s32 savedIndex;
    s32 delta = 0, dx = 0, dz = 0, speed, step;
    struct SceneCell *cell;
    u8 *dynamic;
    if (({
            u8 active = 0;
            if (gPlayerPassageState && gPlayerInteractionMode == 2)
                active = 1;
            active;
        }) ||
        actor->type == 1) {
        step = 16;
        speed = 16;
    } else {
        step = 4;
        speed = 64;
    }
    switch (direction) {
        case 0:
            dz = -step;
            delta = -((s16 *)gTerrainMap->cells)[2];
            break;
        case 1:
            dx = step;
            delta = 1;
            break;
        case 2:
            dz = step;
            delta = ((s16 *)gTerrainMap->cells)[2];
            break;
        case 3:
            dx = -step;
            delta = -1;
            break;
    }
    index += delta;
    dynamic = TerrainModifiers_FindCellOverride(index, 1);
    savedIndex = index;
    if (dynamic)
        cell = (struct SceneCell *)(dynamic + 4);
    else
        cell = (struct SceneCell *)(gTerrainMap->cells + 12 + savedIndex * 4);
    if (cell) {
        u32 heightByte = cell->height;
        u32 mask = 0xF0;
        s32 type = heightByte >> 4;
        asm("" : : "r"(heightByte));
        asm("" : "+r"(mask));
        switch (type) {
            case 0: {
                u32 lowMask = 15;
                u32 referenceHeight;
                s32 next;
                s32 current;
                referenceHeight = ({
                    struct SceneCell *p = reference;
                    p->height;
                });
                asm("" : : "r"(referenceHeight));
                next = heightByte & lowMask;
                current = referenceHeight & lowMask;
                if ((u32)next >= (u32)current)
                    break;
                if (cell->flags & 8)
                    break;
                if (current - next > 1) {
                    actor->targetY = ({
                        u32 y = next << 8;
                        y;
                    });
                    actor->direction = ({
                        u32 d = direction;
                        d;
                    });
                    {
                        register void (*callback)(struct SceneObject *) = Scene_UpdateObjectStep;
                        actor->update = callback;
                    }
                    actor->timer = 0;
                    actor->entered = 1;
                    actor->dx = dx;
                    actor->dz = dz;
                    actor->kind = 2;
                    actor->cellRef.index = savedIndex;
                    actor->duration = speed;
                    return 3;
                } else {
                    u32 referenceHeight;
                    actor->targetY = ({
                        u32 y = next << 8;
                        y;
                    });
                    referenceHeight = ({
                        struct SceneCell *p = reference;
                        p->height;
                    });
                    {
                        register s32 remaining asm("r0") = referenceHeight & lowMask;
                        if (remaining > 0)
                            remaining--;
                        mask &= referenceHeight;
                        mask |= remaining;
                        reference->height = mask;
                    }
                    {
                        u32 tile = cell->tile;
                        reference->tile = tile;
                    }
                    actor->direction = ({
                        u32 d = direction;
                        d;
                    });
                    {
                        register void (*callback)(struct SceneObject *) = Scene_UpdateObjectStep;
                        actor->update = callback;
                    }
                    actor->timer = 0;
                    actor->entered = 1;
                    actor->dx = dx;
                    actor->dz = dz;
                    actor->kind = 1;
                    actor->cellRef.index = savedIndex;
                    actor->duration = speed;
                    return 1;
                }
            }
            default: {
                /* Preserve the original range normalization for non-flat terrain. */
                s32 normalized = type - 1;
                asm("" : : "r"(normalized));
                break;
            }
        }
    }
    return 0;
}

static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline struct SceneCell *getCell(s32 index)
{
    u8 *dynamic = TerrainModifiers_FindCellOverride(index, 1);
    struct SceneCell *cell;
    if (dynamic)
        cell = (struct SceneCell *)(dynamic + 4);
    else
        cell = (struct SceneCell *)(gTerrainMap->cells + 12 + index * 4);
    return cell;
}

s32 SceneObjects_TryPushNeighbor(struct SceneVec *position, s32 direction)
{
    s32 delta, index;
    struct SceneCell *origin;
    struct SceneObjectManager *manager = gSceneObjectManager;
    struct SceneObject *p;
    s32 i;
    if (!manager)
        return -1;
    origin = getCell(cellIndex(position));
    index = cellIndex(position);
    delta = 0;
    switch (direction) {
        case 0:
            delta = -((s16 *)gTerrainMap->cells)[2];
            break;
        case 1:
            delta = 1;
            break;
        case 2:
            delta = ((s16 *)gTerrainMap->cells)[2];
            break;
        case 3:
            delta = -1;
            break;
    }
    p = manager->objects;
    for (i = 0; i < manager->count; p++, i++) {
        struct SceneCell *reference;
        s32 neighbor, result;
        u32 neighborHeight;
        if (!p->active || p->kind)
            continue;
        reference = &p->cellRef.cell;
        neighbor = index + delta;
        if (neighbor != p->cellRef.index)
            continue;
        if ((origin->height & 15) + 1 != (reference->height & 15))
            continue;
        neighborHeight = getCell(neighbor)->height & 15;
        if (neighborHeight != (reference->height & 15))
            continue;
        if (p->pushLimited && !p->pushesLeft)
            break;
        result = Scene_TryStepDown(p, p->cellRef.index, direction);
        if (result > 0) {
            SceneObjects_PlayPushSound(p->type);
            if (p->type == 1)
                result = -1;
            if (p->pushesLeft)
                p->pushesLeft--;
        }
        return result;
    }
    return 0;
}

extern const u8 gUnk_086049AC[];

/* Try pushing the neighbor in the direction named by table entry `index`. */

s32 SceneObjects_TryPushNeighborByIndex(struct SceneVec *position, u32 index)
{
    return SceneObjects_TryPushNeighbor(position, gUnk_086049AC[index]);
}

/* Idle state: wait until health runs out, then disappear (type 1) or get removed. */

void Scene_UpdateObjectIdle(struct SceneObject *p)
{
    u8 t = p->type;
    if (t == 1) {
        if (p->health <= 0) {
            void *f = Scene_UpdateObjectDisappearance;
            p->update = f;
            p->timer = 0;
            p->entered = t;
            p->kind = 3;
        }
    } else {
        if (p->health <= 0) {
            p->mode = 3;
            p->update = 0;
        }
    }
}

void Scene_UpdateObjectStep(struct SceneObject *p)
{
    struct SceneBody *body = &p->body;
    struct SceneCell *terrain = &p->cellRef.cell;
    if (p->entered) {
        p->entered = 0;
        terrain->flags |= 1;
    }
    body->vx = p->dx;
    body->vz = p->dz;
    body->position.y = (terrain->height & 15) << 8;
    if (p->kind == 2)
        body->position.y -= 256;
    p->center.y = body->position.y;
    Collider_SetPosition(&p->stepCollider, &p->body.position, 0);
    Collider_Refresh(&p->stepCollider);
    if (p->type != 1 && !(p->timer & 3))
        Scene_SpawnDirectionalDust(p);
    if (++p->timer >= p->duration) {
        s32 height = terrain->height & 15;
        if (height < 15)
            height++;
        terrain->height = (terrain->height & 0xF0) | height;
        terrain->tile = 255;
        if (p->type == 1) {
            if (p->kind == 2) {
                {
                    register void (*callback)(struct SceneObject *) = Scene_UpdateFallingObject;
                    p->update = callback;
                }
                p->timer = 0;
                p->entered = 1;
                p->kind = 2;
                p->dx = 0;
                p->dz = 0;
                p->direction = 0;
            } else if (!Scene_TryStepDown(p, p->cellRef.index, p->direction)) {
                Scene_PlayNeighborMovementEffect(p);
                {
                    register void (*callback)(struct SceneObject *) = Scene_UpdateObjectIdle;
                    p->update = callback;
                }
                p->timer = 0;
                p->entered = 1;
                p->kind = 0;
                p->dx = 0;
                p->dz = 0;
                p->direction = 0;
                terrain->flags &= ~1;
            }
        } else {
            if (p->kind == 2) {
                {
                    register void (*callback)(struct SceneObject *) = Scene_UpdateFallingObject;
                    p->update = callback;
                }
                p->timer = 0;
                p->entered = 1;
                p->kind = 2;
            } else {
                Scene_PlayNeighborMovementEffect(p);
                {
                    register void (*callback)(struct SceneObject *) = Scene_UpdateObjectIdle;
                    p->update = callback;
                }
                p->timer = 0;
                p->entered = 1;
                p->kind = 0;
                terrain->flags &= ~1;
            }
            p->dx = 0;
            p->dz = 0;
            p->direction = 0;
        }
    }
}

void Scene_UpdateFallingObject(struct SceneObject *p)
{
    struct SceneBody *body = &p->body;
    struct SceneCell *terrain = &p->cellRef.cell;
    if (p->entered) {
        s32 height;
        p->entered = 0;
        height = (p->targetY >> 8) + 1;
        if (height > 15)
            terrain->height = (terrain->height & 0xF0) | 15;
        else
            terrain->height = (terrain->height & 0xF0) | height;
        if (({
                u32 mask = 8;
                mask & p->flags;
            }))
            p->cellRef.cell.flags |= 0x21;
    }
    body->vy = p->targetY - body->position.y;
    if (body->vy < -16)
        body->vy = -16;
    else if (body->vy > 0)
        body->vy = 0;
    if (body->vy == 0) {
        if (({
                u32 mask = 8;
                mask & p->flags;
            })) {
            p->cellRef.cell.flags |= 0x21;
            p->health = 0;
        } else {
            p->cellRef.cell.flags &= ~0x21;
            Scene_PlayMovementEffect(p);
        }
        {
            register void (*callback)(struct SceneObject *) = Scene_UpdateObjectIdle;
            p->update = callback;
        }
        p->timer = 0;
        p->entered = 1;
        p->dx = 0;
        p->dz = 0;
        p->direction = 0;
        p->kind = 0;
    }
}

struct Args {
    u32 count : 16;
    s32 *values;
};

s32 Mod(s32, s32);

static inline void setMode(struct SceneDraw *draw, s16 mode)
{
    draw->mode = mode;
}

void Scene_UpdateObjectDisappearance(struct SceneObject *p)
{
    s32 values[5];
    struct Args args;
    if (p->entered) {
        p->entered = 0;
        p->phase = 0;
        setMode(&p->draw, 0);
        Sound_PlaySong(0x136);
        p->cellRef.cell.flags |= 0x21;
        if (p->script) {
            values[0] = p->body.id;
            values[1] = p->type;
            values[2] = p->body.position.x;
            values[3] = p->body.position.y;
            values[4] = p->body.position.z;
            args.count = 5;
            args.values = values;
            Script_Call(p->script, &args);
            p->script = 0;
        }
    }
    if (!p->phase) {
        if (!Mod(p->timer, 2)) {
            if (!p->draw.mode)
                setMode(&p->draw, 1);
            else
                setMode(&p->draw, 0);
        }
        if (p->timer++ > 29) {
            p->timer = 0;
            p->phase++;
        }
    } else {
        if (p->timer++ > 5) {
            p->timer = 0;
            if (++p->phase > 3)
                p->mode = 2;
            else
                setMode(&p->draw, p->phase);
        }
    }
}

void Scene_UpdateObjectFlash(struct SceneObject *p)
{
    if (p->entered) {
        p->entered = 0;
        p->phase = 3;
        setMode(&p->draw, 3);
        Sound_PlaySong(0x136);
    }
    if (p->phase) {
        if (p->timer++ > 5) {
            p->timer = 0;
            p->phase--;
            setMode(&p->draw, p->phase);
        }
    } else {
        register void (*callback)(struct SceneObject *) = Scene_UpdateObjectIdle;
        p->update = callback;
        p->timer = 0;
        p->entered = 1;
        p->kind = 0;
    }
}

extern void (*const gUnk_086049B4[])(struct SceneObject *);

void Obj_StepPosition(void *);

void sub_0821DE30(void *);

void Palette_InterpolateBanks(void *, u32, u32, s32, s32);

static inline u8 ownerIsCurrent(u32 owner)
{
    if (gGameState->region == (s32)owner)
        return 1;
    return 0;
}

static inline void disableCollision(u16 *flags)
{
    u32 value = *flags;
    u32 state = 4;
    state |= value;
    *flags = state;
}

s32 SceneObjects_Update(struct SceneObjectManager *manager)
{
    struct SceneObject *p = manager->objects;
    s32 i;
    s32 values[5];
    struct Args args;
    for (i = 0; i < manager->count; i++, p++) {
        if (!p->active)
            continue;
        if (p->mode) {
            if (p->mode == 3 && gUnk_086049B4[p->type])
                gUnk_086049B4[p->type](p);
            if (p->script) {
                values[0] = p->body.id;
                values[1] = p->type;
                values[2] = p->body.position.x;
                values[3] = p->body.position.y;
                values[4] = p->body.position.z;
                args.count = 5;
                args.values = values;
                Script_Call(p->script, &args);
                p->script = 0;
            }
            Scene_DropObjectsAbove(p);
            SceneObjects_DestroyObject(p);
            continue;
        }
        Obj_StepPosition(&p->body);
        if (p->update)
            p->update(p);
        if (p->flags & 0x20) {
            if (!p->kind) {
                p->body.flags &= ~2;
                sub_0821DE30(&p->link);
            } else
                p->body.flags |= 2;
        } else
            sub_0821DE30(&p->link);
        if (p->cooldown)
            p->cooldown--;
        if (!p->owner || ownerIsCurrent(p->owner))
            p->draw.flags &= ~1;
        else
            p->draw.flags |= 1;
        if (p->damageTimer || p->health <= 0) {
            p->damageTimer--;
            goto disable;
        }
        if (!p->owner || ownerIsCurrent(p->owner)) {
            p->collider.state &= ~4;
            Collider_SetPosition(&p->collider, &p->body.position, 0);
        } else {
        disable:
            disableCollision(&p->collider.state);
        }
        if (p->animationTimer && !--p->animationTimer)
            AnimData_SelectPaletteBank(&p->animationData, p->basePalette);
    }
    if (manager->fading) {
        Palette_InterpolateBanks(manager->paletteA, manager->fromA, manager->toA, manager->fade, 5);
        Palette_InterpolateBanks(manager->paletteB, manager->fromB, manager->toB, manager->fade, 5);
        if (!manager->descending) {
            if (++manager->fade > 31) {
                manager->fade = 32;
                manager->descending = 1;
            }
        } else if (--manager->fade <= 0) {
            manager->fade = 0;
            manager->descending = 0;
            if (manager->stopFade)
                manager->stopFade = 0;
        }
    }
    manager->frame = 1 - manager->frame;
    return 0;
}

/* Task teardown: free all objects, release the pool and clear the global manager. */

u32 SceneObjects_DestroyTask(struct SceneObjectManager *manager)
{
    SceneObjects_DestroyAllObjects();
    Heap_Free((u32)manager->objects);
    return (u32)(gSceneObjectManager = 0);
}

/* Allocate the object pool (default 8 objects) and initialize the palette-fade state. */

s32 SceneObjects_InitPool(struct SceneObjectManager *p, void *unused)
{
    void *m;
    u8 *b;
    gSceneObjectManager = p;
    if (p->count == 0)
        p->count = 8;
    m = Heap_Alloc(p->count * sizeof(struct SceneObject));
    p->objects = m;
    if (m == 0)
        return -1;
    Memory_Clear(m, p->count * sizeof(struct SceneObject));
    p->frame = 0;
    p->fading = 0;
    p->descending = 0;
    p->fade = 0;
    b = gPaletteBankData;
    p->fromA = (u32)(b + 0x80);
    p->fromB = (u32)(b + 0xa0);
    p->toA = (u32)(b + 0xc0);
    p->toB = (u32)(b + 0xe0);
    CpuSet((void *)p->paletteA[0], &p->paletteA[0], 0x04000008);
    CpuSet((void *)p->paletteA[1], p->paletteB, 0x04000008);
    return 0;
}

/* Create the scene-object task (or return the existing one). */

struct SceneObjectManager *SceneObjects_CreateTask(void *a)
{
    struct SceneObjectManager *p;
    if (gSceneObjectManager != 0)
        return gSceneObjectManager;
    p = Task_Create(8, 0x74);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void))SceneObjects_Update,
                          (void (*)(void))SceneObjects_DestroyTask);
        p->count = Script_GetKeywordValue(0x6d, 8);
        if (SceneObjects_InitPool(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

/* Find a free object slot, or NULL. */

struct SceneObject *SceneObjects_FindFreeSlot(struct SceneObjectManager *manager)
{
    struct SceneObject *e = manager->objects;
    s32 i;
    for (i = 0; i < manager->count; e++, i++) {
        if (e->active == 0)
            return e;
    }
    return 0;
}

/* Find the active object with the given script id, or NULL. */

struct SceneObject *SceneObjects_FindByScriptId(struct SceneObjectManager *manager, u32 id)
{
    struct SceneObject *e = manager->objects;
    s32 i;
    for (i = 0; i < manager->count; e++, i++) {
        if (e->active != 0 && e->body.id == id)
            return e;
    }
    return 0;
}

void SceneBody_Init(struct SceneBody *, u32, struct SceneVec *, s32, s32, struct SceneObject *);

void SceneBody_SetSprite(struct SceneBody *, struct SceneDraw *);

u32 Region_FindContaining(struct SceneVec *);

u32 TerrainModifiers_RegisterCell(void *, u32, u32, u32, u32, u32);

void Collider_SetAttackPowerAndForce(struct SceneCollider *, s32, s32);

s32 TerrainOverlay_QueueAddActor(struct SceneBody *);

static inline void initializeCell(void *cell, s32 index, s32 height)
{
    TerrainModifiers_RegisterCell(cell, index, 0, height, 255, ({
                                      register s32 zero asm("r2") = 0;
                                      zero;
                                  }));
}

static inline void setStepFlags(struct SceneCollider *c)
{
    c->contactFlags = 0x800;
    c->contactFlags2 = 0;
}

static inline u16 flags16(struct SceneObject *p, u32 mask)
{
    return mask & p->flags;
}

// MATCHING_CONCESSION: register-pinning
s32 SceneObjects_CreateObject(u32 id, s32 type, u32 flags, struct SceneVec *position,
                              u32 animationResource, s32 drawMode, s32 health, s32 basePalette,
                              s32 animation, u32 script)
{
    struct SceneExtent size, offset;
    struct SceneObjectManager *manager = gSceneObjectManager;
    struct SceneObject *p;
    register struct SceneDraw *draw asm("r5");
    register struct SceneVec *pos asm("r9");
    s32 x, z, index, height;
    u32 clear;
    u8 *dynamic;
    struct SceneCell *cell;
    if (!manager || !(p = SceneObjects_FindFreeSlot(manager)))
        return -1;
    Memory_Clear(p, 0x1A8);
    p->active = 1;
    p->type = type;
    if (p->type == 6)
        manager->fading = 1;
    p->flags = flags;
    p->timer = 0;
    p->kind = 0;
    AnimData_Load(&p->animationData, animationResource);
    draw = &p->draw;
    asm("" : "+r"(draw));
    Sprite_Init(draw, &p->animationData, 0);
    draw->mode = drawMode;
    p->center.x = (position->x & ~255) + 128;
    p->center.y = position->y & ~255;
    p->center.z = (position->z & ~255) + 128;
    SceneBody_Init(&p->body, id, position, 0, 7, p);
    SceneBody_SetSprite(&p->body, draw);
    p->body.position = p->center;
    if (flags16(p, 16)) {
        p->owner = Region_FindContaining(&p->body.position);
        pos = &p->body.position;
    } else {
        p->owner = 0;
        pos = &p->body.position;
    }
    p->dx = 0;
    p->dz = 0;
    p->direction = 0;
    if (flags16(p, 4)) {
        {
            register void (*callback)(struct SceneObject *) asm("r0") = Scene_UpdateObjectFlash;
            asm("" : "+r"(callback));
            p->update = callback;
        }
        p->timer = 0;
    } else {
        {
            register void (*callback)(struct SceneObject *) asm("r0") = Scene_UpdateObjectIdle;
            asm("" : "+r"(callback));
            p->update = callback;
        }
        p->timer = 0;
    }
    p->entered = 1;
    x = p->body.position.x >> 8;
    z = ({
            register struct SceneVec *coords asm("r1") = pos;
            asm("" : "+r"(coords));
            coords->z;
        }) >>
        8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        index = 0;
    else
        index = gTerrainMap->rows[z] + x;
    dynamic = TerrainModifiers_FindCellOverride(index, 1);
    if (dynamic)
        cell = (struct SceneCell *)(dynamic + 4);
    else
        cell = (struct SceneCell *)(gTerrainMap->cells + 12 + index * 4);
    height = cell->height & 15;
    if (height < 15)
        height++;
    initializeCell(&p->cellRef, index, height);
    {
        register u32 y asm("r0") = height << 8;
        asm("" : "+r"(y));
        clear = ({
            register u32 zero asm("r3") = 0;
            asm("" : "+r"(zero));
            zero;
        });
        p->targetY = y;
    }
    size.x = 130;
    size.y = 128;
    size.z = 130;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    Collider_Init(&p->collider, p->body.id, 0x4041, 0, 16, &size, &offset);
    Collider_SetPosition(&p->collider, pos, 0);
    Collider_SetCallback(&p->collider, Scene_ApplyTerrainInteraction, p);
    Collider_Refresh(&p->collider);
    ((u16 *)&size)[0] = 128;
    ((u16 *)&size)[1] = 128;
    ((u16 *)&size)[2] = 128;
    Collider_Init(&p->stepCollider, p->body.id, 0x2801, 0, 1, &size, &offset);
    Collider_SetPosition(&p->stepCollider, pos, 0);
    Collider_SetCallback(&p->stepCollider, SceneObjects_NoOpUpdate, p);
    Collider_SetAttackPowerAndForce(&p->stepCollider, 1500, 50);
    setStepFlags(&p->stepCollider);
    p->health = health;
    p->damageTimer = clear;
    p->mode = clear;
    p->animationTimer = clear;
    p->phase = clear;
    p->cooldown = clear;
    p->unk23 = 4;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    p->randomPhase = Mod(({
                             u32 offset = gRandomIndex * 2;
                             register u16 *table asm("r2");
                             asm("" : "+r"(offset));
                             table = gRandomTable;
                             asm("" : "+r"(table));
                             *(u16 *)(offset + (u32)table);
                         }),
                         16);
    {
        register s32 initial asm("r3") = basePalette;
        asm("" : "+r"(initial));
        if (initial >= 0)
            p->basePalette = initial;
        else
            p->basePalette = p->animationData.paletteIndex;
    }
    AnimData_SelectPaletteBank(&p->animationData, p->basePalette);
    p->animation = animation;
    p->link.id = p->body.id;
    p->link.position = pos;
    p->script = script;
    if (!(p->flags & 2) && TerrainOverlay_QueueAddActor(&p->body) < 0)
        p->flags |= 2;
    if (p->type == 5) {
        p->pushLimited = 1;
        p->pushesLeft = 1;
    } else {
        p->pushLimited = 0;
        p->pushesLeft = 0;
    }
    return 0;
}

s32 SceneObjects_CreateFromScript(void)
{
    struct SceneExtent position;
    s32 id, type;
    s32 initial, animation, script;
    s32 flags, resource, drawMode, health;
    id = Script_GetKeywordValue('n', 0);
    type = Script_GetKeywordValue('t', 0);
    flags = Script_GetKeywordValue('f', 0);
    if (Script_SeekToKeyword('M') && Script_GetValue())
        flags |= 16;
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    if (type == 0)
        resource = Script_GetKeywordValue('a', 0x9D41);
    else if (type == 1)
        resource = Script_GetKeywordValue('a', 0x84EE);
    else if (type == 2)
        resource = Script_GetKeywordValue('a', 0x9D40);
    else if (type == 3)
        resource = Script_GetKeywordValue('a', 0x9D43);
    else if (type == 4)
        resource = Script_GetKeywordValue('a', 0xE0C9);
    else if (type == 5)
        resource = Script_GetKeywordValue('a', 0x5A2);
    else if (type == 6)
        resource = Script_GetKeywordValue('a', 0xB306);
    else
        resource = Script_GetKeywordValue('a', 0x9D41);
    drawMode = Script_GetKeywordValue('i', 0);
    health = Script_GetKeywordValue('l', 128);
    if (type == 0)
        initial = 0x2B8;
    else if (type == 1)
        initial = 0x2C2;
    else if (type == 2)
        initial = 0x279;
    else if (type == 3)
        initial = 0x6E;
    else if (type == 4)
        initial = 0x1D0;
    else if (type == 5)
        initial = 0xC4;
    else if (type == 6) {
        if (drawMode == 0 || drawMode == 2)
            initial = 4;
        else
            initial = 5;
    } else
        initial = Script_GetKeywordValue('d', -1);
    animation = 0x192;
    script = Script_GetKeywordValue('b', 0);
    SceneObjects_CreateObject(id, type, flags, (struct SceneVec *)&position, resource, drawMode,
                              health, initial, animation, script);
    return 0;
}

s32 SceneObjects_SwitchPaletteFromScript(void)
{
    struct SceneObjectManager *manager = gSceneObjectManager;
    struct SceneObject *p;
    struct SceneDraw *draw;
    s32 id, mode, sound;
    if (!manager || !(id = Script_GetKeywordValue('n', 0)) ||
        !(p = SceneObjects_FindByScriptId(manager, id)) || p->type != 6)
        return -1;
    draw = &p->draw;
    mode = Script_GetKeywordValue('s', 0);
    sound = Script_GetKeywordValue('S', 1);
    if (!mode) {
        if (draw->mode == 0 || draw->mode == 2) {
            p->basePalette = 4;
            AnimData_SelectPaletteBank(&p->animationData, 4);
        } else {
            p->basePalette = 5;
            AnimData_SelectPaletteBank(&p->animationData, 5);
        }
    } else {
        if (draw->mode == 0 || draw->mode == 2) {
            p->basePalette = 6;
            AnimData_SelectPaletteBank(&p->animationData, 6);
            p->animationData.palette = manager->paletteA;
        } else {
            p->basePalette = 7;
            AnimData_SelectPaletteBank(&p->animationData, 7);
            p->animationData.palette = manager->paletteB;
        }
        manager->stopFade = 1;
        if (sound)
            Sound_PlaySong(0x149);
    }
}
