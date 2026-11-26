#include "global.h"
#define RisingSprite_CreatePool RisingPuffPool_GetOrCreate
struct RisingSpritePosition {
    s16 x, y, z, pad;
};
struct RisingSprite {
    u32 flags;
    u8 unknown4[0x14];
    struct RisingSpritePosition position;
    u8 unknown20[8];
    void *resource;
    u8 active, unknown2D;
    u16 frame;
};
struct RisingSpritePool {
    u8 task[0x18];
    struct RisingSprite sprites[8];
    u16 next;
};
extern struct RisingSpritePool *gRisingSpritePool;
void *RisingPuffPool_GetOrCreate(void);
void RisingSprite_Spawn(struct RisingSpritePosition *position)
{
    struct RisingSprite *p;
    if (!gRisingSpritePool)
        RisingPuffPool_GetOrCreate();
    p = &gRisingSpritePool->sprites[gRisingSpritePool->next];
    p->flags &= ~1;
    p->position = *position;
    p->frame = 0;
    p->active = 1;
    gRisingSpritePool->next++;
    if (gRisingSpritePool->next > 7)
        gRisingSpritePool->next = 0;
}
