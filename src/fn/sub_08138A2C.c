#define SpriteSlots_Create PositionEffectDisplay_Create
#define gSpriteSlots gUnk_020004C0
#include "global.h"
struct SpriteSlot {
    u32 flags;
    u8 unk04[0x38];
    void *source;
    u8 unk40[2];
    u16 active;
    u8 unk44[0x1c];
};
struct SpriteSlots {
    u8 unk00[0x34];
    struct SpriteSlot slots[4];
};
extern struct SpriteSlots *gUnk_020004C0;
struct SpriteSlots *PositionEffectDisplay_Create(void);
s32 SpriteSlots_AttachSource(void *source)
{
    struct SpriteSlots *initial = gUnk_020004C0;
    struct SpriteSlots *p;
    s32 i;
    if (!initial) {
        p = PositionEffectDisplay_Create();
        if (!p) return -1;
    } else {
        p = initial;
    }
    for (i = 0; i < 4; i++) {
        if (!p->slots[i].active) {
            p->slots[i].source = source;
            p->slots[i].flags &= ~1;
            p->slots[i].active = 1;
            return i;
        }
    }
    return -1;
}
