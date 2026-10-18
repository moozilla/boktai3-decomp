#include "global.h"
#define RisingSprite_Spawn      sub_08237368
#define RisingSprite_CreatePool RisingPuffPool_GetOrCreate
#define gRisingSpritePool       gUnk_020004A0
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
extern struct RisingSpritePool *gUnk_020004A0;
void *RisingPuffPool_GetOrCreate(void);
void sub_08237368(struct RisingSpritePosition *position)
{
    struct RisingSprite *p;
    if (!gUnk_020004A0)
        RisingPuffPool_GetOrCreate();
    p = &gUnk_020004A0->sprites[gUnk_020004A0->next];
    p->flags &= ~1;
    p->position = *position;
    p->frame = 0;
    p->active = 1;
    gUnk_020004A0->next++;
    if (gUnk_020004A0->next > 7)
        gUnk_020004A0->next = 0;
}
