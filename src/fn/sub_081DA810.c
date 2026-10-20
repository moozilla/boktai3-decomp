#include "contact_swarm.h"
// confidence=low: allocates a timed-effect entry for the projectile configuration.
struct Config {
    u16 flags, strength, tracking;
    s8 direction;
    u8 turnSpeed;
    s32 speed;
    u32 unknownC;
    s32 (*callback)(void *, s32);
    u32 mode, contactFlags, resourceId, cost, option;
    struct ContactPosition bounds, offset;
    s16 category;
    u16 unknown3A;
    u32 graphics, frame;
    s32 palette, type;
    u8 unknown4C[6];
    u16 collisionFlags;
    u32 unknown54;
};
struct Entry {
    u32 frame, unknown4;
    u8 unknown8[8];
    struct ContactPosition velocity;
    struct Config config;
    struct ContactSwarmGraphics graphics;
    struct ContactSwarmSprite sprite;
    struct ContactCollider collider;
    u8 unknown10C[0x164 - 0x10C];
    s32 lifetime, elapsed;
    u8 unknown16C[4];
    s32 speed;
    struct ContactPosition origin;
    u16 orientation;
    u8 unknown17E[0x12];
    u32 counter;
    u8 unknown194[0x12];
    u16 hit;
};
struct Manager {
    u8 task[0x18];
    u32 activeMask;
    s32 categoryCounts[4], totalCounts[4];
};
extern void (*const gUnk_086136A8[])(void);
extern const s16 gUnk_086149C4[];
struct Entry *TimedEffect_FindFreeEntry(struct Manager *, s32 *);
void Projectile_SetUpdateCallback(void *, void (*)(void));
void Projectile_InitFromType(void *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Collider_SetCallback(void *, void *, void *);
void sub_081DA048(void);
void Sprite_LinkToList(void *, s32);
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
s32 TimedEffect_SpawnProjectile(struct Manager *manager, struct Config *config,
                                const struct ContactPosition *position)
{
    s32 index;
    struct ContactPosition size, offset;
    struct Entry *entry;
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
    entry->elapsed = 0;
    if (config->type == 9)
        config->speed = 18;
    entry->orientation = (((u16)config->direction + 0x70) >> 5) & 7;
    entry->speed = config->speed;
    config->turnSpeed = 3;
    if (config->type == 8) {
        config->speed += config->speed >> 1;
        config->strength = (config->strength >> 1) + (config->strength >> 3);
    }
    Memory_Copy(&entry->config, config, 0x58);
    entry->frame = 0;
    entry->counter = 0;
    velocity = &entry->velocity;
    direction = config->direction;
    speed = config->speed;
    velocity->x = Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
    velocity->y = 0;
    velocity->z = Scale(gUnk_086149C4[direction & 255] * speed);
    collider = &entry->collider;
    special = (u16)(config->flags & 0x8000);
    saved = collider;
    if (special) {
        Collider_Init(collider, 0, 0x2101, 0, 16, &config->bounds, &config->offset);
        entry->hit = 0;
    } else {
        Collider_Init(collider, 0, 0x2101, 0, config->collisionFlags, &config->bounds,
                      &config->offset);
        entry->hit = 0;
    }
    if (config->type == 9) {
        collider = saved;
        size.x = 8;
        size.y = 8;
        size.z = 8;
        offset.x = 0;
        offset.y = 0;
        offset.z = 0;
        Collider_Init(collider, 0, 0x2002, 0, config->collisionFlags, &size, &offset);
        Collider_SetAttackProperties(collider, 0, 0, config->mode, config->contactFlags, 0);
        Collider_SetPosition(collider, &entry->sprite.position, 0);
        Collider_SetCallback(collider, sub_081DA048, manager);
    } else {
        collider = saved;
        Collider_SetAttackProperties(collider, config->resourceId, config->cost, config->mode,
                                     config->contactFlags, config->option);
        Collider_SetCallback(collider, sub_081DA048, entry);
        {
            s32 heading = config->direction;
            *((s8 *)entry + 0xFA) = heading;
        }
        Collider_SetPosition(collider, position, 0);
    }
    entry->sprite.position = *position;
    entry->origin = *position;
    graphics = &entry->graphics;
    AnimData_Load(graphics, config->graphics);
    sprite = &entry->sprite;
    sprite->frame = config->frame;
    if (config->palette >= 0)
        AnimData_SelectPaletteBank(graphics, config->palette);
    sprite->flags &= ~1;
    Sprite_LinkToList(sprite, 0);
    {
        u8 *priority = (u8 *)entry + 0x93;
        u32 one = 1;
        *priority = one;
        manager->activeMask |= one << index;
    }
    if (!config->callback)
        return -1;
    sub_08249240(entry, (void *)index, config->callback);
    if (!(config->flags & 0x2010))
        manager->categoryCounts[config->category]++;
    manager->totalCounts[config->category]++;
    return index;
}
