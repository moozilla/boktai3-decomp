#include "global.h"
#include "engine.h"

struct TConfig {
    u8 pad0[0x14];
    u32 mode, contactFlags, resourceId, cost, option;
    u8 pad28[0x3C - 0x28];
    u32 graphics;
    u8 pad40[0x58 - 0x40];
};
struct TEntry {
    u32 frame;
    u8 pad4[0xC];
    u32 vel[2];
    struct TConfig config;
    u8 graphics[0x1C];
    u8 sprite[0x2C];
    u8 collider[0x190 - 0xB8];
    u32 f190;
    u8 pad194[0x1A8 - 0x194];
    s32 index;
};

void Projectile_InitializeParams(void *);
void Sprite_InitUnlinked(void *, void *, u32);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void sub_081DA048(void);

static inline void FillWords(u32 *dst, int ctl, u32 *src)
{
    CpuSet(src, dst, ctl);
}

void TimedEffect_InitEntry(void *manager, struct TEntry *entry, s32 index)
{
    struct TConfig *config = &entry->config;
    void *collider;
    u32 zero;
    u32 *vel;

    Projectile_InitializeParams(config);
    entry->frame = 0;
    entry->f190 = 0;
    entry->index = index;
    vel = entry->vel;
    zero = 0;
    FillWords(vel, 0x05000002, &zero);
    AnimData_Load(entry->graphics, config->graphics);
    Sprite_InitUnlinked(entry->sprite, entry->graphics, 1);
    collider = entry->collider;
    if (*(u16 *)entry->config.pad0 & 0x8000)
        Collider_Init(collider, 0, 0x2101, 0, 16, (u8 *)entry + 0x40, (u8 *)entry + 0x48);
    else
        Collider_Init(collider, 0, 0x2101, 0, 1, (u8 *)entry + 0x40, (u8 *)entry + 0x48);
    Collider_SetCallback(collider, sub_081DA048, entry);
    Collider_SetAttackProperties(collider, config->resourceId, config->cost, config->mode,
                                 config->contactFlags, config->option);
}
