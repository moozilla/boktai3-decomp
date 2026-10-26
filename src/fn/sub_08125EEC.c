#include "global.h"
#include "ram.h"
#define SpriteBurst_AllocateEntry      SceneMovingParticle_AllocateEntry
#define SpriteBurst_GetOrCreateManager SceneMovingParticle_EnsurePool
struct SpriteBurstPair {
    u32 x, y;
};
struct SpriteBurstEntry {
    u32 flags;
    u8 unknown4[11], mode;
    u8 unknown10[8];
    struct SpriteBurstPair position;
    u8 unknown20[8];
    struct SpriteBurstPair velocity;
    u16 age;
    u8 active, index, state, lifetime, phase, timer;
    u16 kind;
    u8 unknown3A[2], frame, unknown3D[3];
};
struct SpriteBurstManager {
    u8 unknown0[0x18];
    void *resource;
};
struct SpriteBurstManager *SceneMovingParticle_EnsurePool(void);
struct SpriteBurstEntry *SceneMovingParticle_AllocateEntry(struct SpriteBurstManager *);
void ParticleSprite_Initialize(struct SpriteBurstEntry *, void *, u32);
void Effect_SetOrigin(struct SpriteBurstEntry *, s32, s32);
void SpriteDrawable_SelectTileFrame(struct SpriteBurstEntry *, void *, u32);
void Effect_SetPalette(struct SpriteBurstEntry *, u32);
struct SpriteBurstEntry *SpriteBurst_Create(struct SpriteBurstPair *pos,
                                            struct SpriteBurstPair *velocity, u32 kind,
                                            u32 direction, u32 mode)
{
    struct SpriteBurstManager *manager = gSpriteBurstManager;
    struct SpriteBurstEntry *entry;
    u32 frame;
    if (!manager) {
        manager = SceneMovingParticle_EnsurePool();
        if (!manager)
            return 0;
    }
    entry = SceneMovingParticle_AllocateEntry(manager);
    if (!entry)
        return 0;
    entry->active = 1;
    entry->age = 0;
    entry->state = 1;
    entry->velocity = *velocity;
    entry->kind = kind;
    entry->lifetime = 30;
    entry->phase = 0;
    switch (direction) {
        case 1:
            frame = 1;
            break;
        case 2:
            frame = 2;
            break;
        case 3:
            frame = 3;
            break;
        case 4:
            frame = 4;
            break;
        case 5:
            frame = 5;
            break;
        case 6:
            frame = 6;
            break;
        case 7:
            frame = 7;
            break;
        case 0:
        default:
            frame = 0;
            break;
    }
    entry->timer = 0;
    entry->frame = frame * 2;
    entry->position = *pos;
    ParticleSprite_Initialize(entry, manager->resource, 0);
    Effect_SetOrigin(entry, -4, -4);
    SpriteDrawable_SelectTileFrame(entry, manager->resource, entry->frame);
    Effect_SetPalette(entry, 1);
    entry->flags |= 1;
    entry->mode = mode;
    return entry;
}
