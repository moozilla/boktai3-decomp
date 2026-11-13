#include "projectile_config.h"

// confidence=low: allocates a timed-effect entry for the projectile configuration.

extern void (*const gUnk_086136A8[])(void);

extern const s16 gUnk_086149C4[];

static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

s32 TimedEffect_SpawnProjectile(struct TimedEffectManager *manager, struct Task4E01Params *config,
                                const struct ContactPosition *position)
{
    s32 index;
    struct ContactPosition size, offset;
    struct Task4E01Projectile *entry;
    struct ContactPosition *velocity;
    struct ContactCollider *collider;
    struct ContactCollider *saved;
    struct ContactSwarmGraphics *graphics;
    struct ContactSwarmSprite *sprite;
    s32 direction, speed;
    u32 special;
    entry = TimedEffect_FindFreeEntry(manager, &index);
    if (!entry)
        return -1;
    Projectile_SetUpdateCallback(config, gUnk_086136A8[config->type]);
    Projectile_InitFromType(config);
    entry->lifetime = 6;
    entry->runtime.elapsed = 0;
    if (config->type == 9)
        config->speed = 18;
    entry->runtime.turn = (((u16)config->direction + 0x70) >> 5) & 7;
    entry->runtime.height = config->speed;
    config->turnSpeed = 3;
    if (config->type == 8) {
        config->speed += config->speed >> 1;
        config->lifetime = (config->lifetime >> 1) + (config->lifetime >> 3);
    }
    Memory_Copy(&entry->params, config, 0x58);
    entry->frame = 0;
    entry->runtime.restarted = 0;
    velocity = &entry->velocity;
    direction = config->direction;
    speed = config->speed;
    velocity->x = Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
    velocity->y = 0;
    velocity->z = Scale(gUnk_086149C4[direction & 255] * speed);
    collider = &entry->colliders[0];
    special = (u16)(config->flags & 0x8000);
    saved = collider;
    if (special) {
        Collider_Init(collider, 0, 0x2101, 0, 16, &config->bounds, &config->offset);
        entry->runtime.cooldown = 0;
    } else {
        Collider_Init(collider, 0, 0x2101, 0, config->unknown52, &config->bounds, &config->offset);
        entry->runtime.cooldown = 0;
    }
    if (config->type == 9) {
        collider = saved;
        size.x = 8;
        size.y = 8;
        size.z = 8;
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        Collider_Init(collider, 0, 0x2002, 0, config->unknown52, &size, &offset);
        Collider_SetAttackProperties(collider, 0, 0, config->damage[0], config->damage[1], 0);
        Collider_SetPosition(collider, &entry->sprite.position, 0);
        Collider_SetCallback(collider, TimedEffect_OnContact, manager);
    } else {
        collider = saved;
        Collider_SetAttackProperties(collider, config->damage[2], config->damage[3],
                                     config->damage[0], config->damage[1], config->damage[4]);
        Collider_SetCallback(collider, TimedEffect_OnContact, entry);
        {
            s32 heading = config->direction;
            entry->colliders[0].direction = heading;
        }
        Collider_SetPosition(collider, position, 0);
    }
    entry->sprite.position = *position;
    entry->runtime.previousPosition = *position;
    graphics = &entry->graphics;
    AnimData_Load(graphics, config->sprite[0]);
    sprite = &entry->sprite;
    sprite->frame = config->sprite[1];
    if ((s32)config->sprite[2] >= 0)
        AnimData_SelectPaletteBank(graphics, config->sprite[2]);
    sprite->flags &= ~1;
    Sprite_LinkToList(sprite, 0);
    {
        u8 *priority = &entry->sprite.priority;
        u32 one = 1;
        *priority = one;
        manager->activeMask |= one << index;
    }
    if (!config->callback)
        return -1;
    sub_08249240(entry, (void *)index, config->callback);
    if (!(config->flags & 0x2010))
        manager->counts[config->variant]++;
    manager->totalCounts[config->variant]++;
    return index;
}

s32 Projectile_Spawn(struct Task4E01Params *params, void *arg)
{
    s32 total = 0;
    struct TimedEffectManager *task;
    s32 i;
    s32 step;
    s32 kind;
    s32 result;
    task = ResourceRegistry_GetResource(0x4E01);
    if (!task) {
        Task4E01_GetOrCreate();
        return -1;
    }
    {
        s32 limit = task->capacity;
        u32 *t = (u32 *)task->totalCounts;
        i = 3;
        do {
            total += *t++;
            i--;
        } while (i >= 0);
        if (total >= limit)
            return -1;
    }
    {
        u32 f = params->flags & 0x2010;
        kind = params->type;
        if (!f) {
            if (kind == 1) {
                if ((u32)task->counts[params->variant] > 4)
                    return -1;
            } else if (kind == 0 || kind == 4 || kind == 3 || kind == 5 || kind == 6) {
                if ((u32)task->counts[params->variant] > 2)
                    return -1;
            } else {
                if (task->counts[params->variant] != 0)
                    return -1;
            }
        }
    }
    if (kind == 2) {
        step = 0x10;
        params->direction -= 0x20;
        for (i = 0; i <= 4; i++) {
            params->flags &= 0xF0FF;
            if (Mod(i, 2) == 0)
                params->flags |= 0x100;
            else
                params->flags |= 0x200;
            result = TimedEffect_SpawnProjectile(task, params, arg);
            params->direction += step;
            if (result == -1)
                return -1;
        }
        return result;
    }
    return TimedEffect_SpawnProjectile(task, params, arg);
}

s32 Projectile_CreateForOwner(struct Task4E01Params *params, void *position, u32 owner)
{
    if (owner > 3) {
        return -1;
    }
    params->variant = owner;
    return Projectile_Spawn(params, position);
}

// The Task4E01 projectile's defaults, in the shape of Projectile_InitParameters.

void Projectile_InitializeParams(struct Task4E01Params *params)
{
    Memory_Clear(params, 0x58);
    Projectile_SetFlightParams(params, 0, 0x3C, 0, 0);
    Projectile_SetDamageParams(params, 0, 0, 0xA, 0x14, 0x14);
    Projectile_SetCollisionSize(params, 8, 0x20, 8);
    Projectile_SetCollisionOffset(params, 0, -0x6C, 0);
    Projectile_SetCollisionMask(params, 1);
    Projectile_SetSpriteParams(params, 0x848F, 0, -1);
    Projectile_SetUpdateCallback(params, Task4E01_OnDefaultTimedAnimation);
    params->flags = 0;
    params->variant = 0;
    params->unknown3A = 0;
}

void Projectile_SetFlightParams(struct Task4E01Params *params, u16 flags, u16 lifetime,
                                s32 direction, u32 speed)
{
    params->flags = flags;
    params->lifetime = lifetime;
    params->direction = direction;
    params->speed = speed;
}

/* Projectile_SetVerticalMotion: Sets the secondary duration and vertical motion fields; precise motion roles tentative */

void Projectile_SetVerticalMotion(struct Task4E01Params *params, u16 tracking, u32 turnParameter,
                                  u8 turnSpeed)
{
    params->tracking = tracking;
    params->turnParameter = turnParameter;
    params->turnSpeed = turnSpeed;
}

void Projectile_SetDamageParams(struct Task4E01Params *params, u32 mode, u32 contactFlags,
                                u32 resource, u32 cost, u32 options)
{
    params->damage[0] = mode;
    params->damage[1] = contactFlags;
    params->damage[2] = resource;
    params->damage[3] = cost;
    params->damage[4] = options;
}

void Projectile_SetCollisionSize(void *input, s32 width, s32 height, s32 depth)
{
    struct Task4E01Params *params = input;
    params->bounds.x = width;
    params->bounds.y = height;
    params->bounds.z = depth;
}

/* Projectile_SetUniformCollisionSize: Sets all three collider dimensions to the same value */

void Projectile_SetUniformCollisionSize(struct Task4E01Params *params, u32 size)
{
    params->bounds.x = size;
    params->bounds.y = size;
    params->bounds.z = size;
}

void Projectile_SetCollisionOffset(struct Task4E01Params *params, s32 x, s32 y, s32 z)
{
    params->offset.x = x;
    params->offset.y = y;
    params->offset.z = z;
}

void Projectile_SetCollisionMask(struct Task4E01Params *params, u16 flags)
{
    params->unknown52 = flags;
}

void Projectile_SetSpriteParams(struct Task4E01Params *params, u32 graphics, u32 frame, s32 palette)
{
    params->sprite[0] = graphics;
    params->sprite[1] = frame;
    params->sprite[2] = palette;
}

void Projectile_SetUpdateCallback(void *input, void *callback)
{
    struct Task4E01Params *params = input;
    params->callback = callback;
}

void Projectile_SetTypeAndElement(struct Task4E01Params *params, u32 type, u32 element)
{
    params->type = type;
    params->element = element;
    if (element > 6) {
        params->element = 0;
    }
}

void Projectile_SetVariant(struct Task4E01Params *params, u16 variant)
{
    params->unknown3A = variant;
}

s32 Task4E01_RemoveEntry(s32 index)
{
    struct TimedEffectManager *manager = ResourceRegistry_GetResource(0x4e01);
    struct Task4E01Projectile *entry;
    if (!manager || index < 0 || index >= manager->capacity ||
        !(manager->activeMask & (1 << index)))
        return 1;
    entry = &manager->entries[index];
    if (entry->params.type == 3 && (entry->params.flags & 0x10))
        --entry->params.parent->count;
    TimedEffect_RemoveFromManager(manager, entry, index);
    return 0;
}

static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}

s32 Projectile_SetEntryDirection(u8 direction, s32 index)
{
    struct TimedEffectManager *task = ResourceRegistry_GetResource(0x4E01);
    struct Task4E01Projectile *entry;
    struct Task4E01Params *params;
    s32 dir;
    s32 speed;
    s16 *v;
    s32 angle;
    if (!task || index < 0 || index >= task->capacity)
        return 1;
    entry = &task->entries[index];
    params = &entry->params;
    {
        u16 pa = entry->params.flags;
        u16 pb = params->lifetime;
        dir = (s8)direction;
        Projectile_SetFlightParams(params, pa, pb, dir, params->speed);
    }
    entry->colliders[0].direction = dir;
    entry->runtime.turn = ((u16)dir + 0x70 >> 5) & 7;
    v = &entry->velocity.x;
    angle = params->direction;
    speed = params->speed;
    {
        s32 sv;
        u32 zero;
        sv = scale(gUnk_086149C4[(angle + 64) & 255] * speed);
        zero = 0;
        v[0] = sv;
        v[1] = zero;
    }
    v[2] = scale(gUnk_086149C4[angle & 255] * speed);
    return 0;
}

struct ContactPosition *Task4E01_GetEntryPosition(s32 index)
{
    struct TimedEffectManager *manager = ResourceRegistry_GetResource(0x4E01);
    if (manager == 0 || index < 0 || index >= manager->capacity)
        return 0;
    return &manager->entries[index].sprite.position;
}

s32 Task4E01_GetKind7Value(s32 index)
{
    struct TimedEffectManager *manager = ResourceRegistry_GetResource(0x4e01);
    struct Task4E01Projectile *entry;
    if (manager && index >= 0 && index < manager->capacity) {
        entry = &manager->entries[index];
        if (entry->params.type == 7)
            return entry->runtime.trailFrame;
    }
    return 0;
}

s32 SceneEntry_SetPosition(s32 index, struct ContactPosition *position)
{
    struct TimedEffectManager *manager = ResourceRegistry_GetResource(0x4e01);
    if (!manager || index < 0 || index >= manager->capacity)
        return -1;
    {
        struct Task4E01Projectile **slot = &manager->entries;
        u32 offset = index * sizeof(struct Task4E01Projectile);
        u8 *base = (u8 *)*slot;
        *(struct ContactPosition *)(base + offset + (u32) &
                                    ((struct Task4E01Projectile *)0)->sprite.position) = *position;
    }
    return 0;
}

s32 Task4E01_Destroy(void)
{
    s32 r;
    struct TimedEffectManager *manager = ResourceRegistry_GetResource(0x4E01);
    if (manager) {
        Task_Kill(manager);
        r = 0;
    } else
        r = -1;
    return r;
}
