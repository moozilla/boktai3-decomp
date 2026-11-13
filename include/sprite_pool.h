#ifndef GUARD_SPRITE_POOL_H
#define GUARD_SPRITE_POOL_H

#include "engine.h"
#include "display_sprite_playback.h"
#include "transient_sprite_pool.h"

struct SpritePool;
struct SpritePoolSlot;
typedef s32 (*SpritePoolCallback)(struct SpritePool *, struct SpritePoolSlot *);

struct SpritePoolSlot {
    u32 id;                                  /* 0x00: slot pointer with generation in high bits */
    u8 active, unused5[3];                   /* 0x04 */
    s32 age;                                 /* 0x08: callback ticks */
    struct ContactSwarmSprite sprite;        /* 0x0C */
    struct TransientSpriteGraphics graphics; /* 0x38 */
    struct DisplaySequenceState sequence;    /* 0x54 */
    void *sequenceResource;                  /* 0x64 */
    SpritePoolCallback callback;             /* 0x68: nonzero result releases the slot */
    u32 userData[16];                        /* 0x6C: private state owned by the callback */
    void *context;                           /* 0xAC */
};

struct SpritePool {
    struct Task task;                /* 0x00 */
    struct SpritePoolSlot slots[16]; /* 0x18 */
    u32 generation;                  /* 0xB18 */
};

s32 SpritePool_Update(struct SpritePool *);
u32 SpritePool_Destroy(struct SpritePool *);
s32 SpritePool_Initialize(struct SpritePool *);
struct SpritePool *SpritePool_Create(void);
struct SpritePoolSlot *SpritePool_FindFreeSlot(struct SpritePool *);
struct SpritePoolSlot *SpritePool_Find(struct SpritePool *, u32 id);
struct SpritePoolSlot *SpritePool_Allocate(void);
void SpritePool_SetResource(struct SpritePoolSlot *, u32 id);
void SpritePool_SetFrame(struct SpritePoolSlot *, u32 frame);
void SpritePool_SetPalette(struct SpritePoolSlot *, u32 palette);
void SpritePool_SetPosition(struct SpritePoolSlot *, const struct ContactPosition *);
void SpritePool_StartAnimation(struct SpritePoolSlot *, u32 resource, u32 sequence, u32 row,
                               u32 flags);
void SpritePool_SetAnimationFrame(struct SpritePoolSlot *, u32 sequence, u32 row, u32 flags);
s32 SpritePool_AdvanceAnimation(struct SpritePoolSlot *);
u32 SpritePool_Activate(struct SpritePoolSlot *, SpritePoolCallback, void *context);
void SpritePool_Remove(u32 id);

void Sprite_InitUnlinked(void *, void *, u32 flags);
void Sprite_ResetRenderDefaults(void *);
s32 Sprite_LinkToList(void *, u32 list);

#endif
