#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Drawable {
    u32 flags;
    u8 active;
    u8 pad[0x13];
    u32 drawFlags;
    u8 tail[0x10];
};
struct Manager {
    u8 task[0x18];
    u16 id, resource;
    s32 initialized, actorCount, drawableCount;
    u32 count;
    u8 commands[64], model[28];
    struct Drawable *drawables;
};
extern const u8 gUnk_0824DAFC[];
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
s32 sub_08219BD8(const u8 *);
s32 sub_082151E4(void *, s32);
void sub_082144E4(struct Drawable *, void *, s32);
void sub_08217060(s32, s32, s32);
void sub_08217074(s32, s32);
void sub_0821A284(u16, void *, s32);
s32 TerrainOverlay_Initialize(struct Manager *p, u32 id, u32 resource)
{
    u8 *model;
    u8 *loadedModel;
    s32 key, i;
    p->id = id;
    p->resource = resource;
    p->initialized = 0;
    p->count = 0;
    if (Script_SeekToKeyword('m')) {
        key = Script_GetValue();
        model = p->model;
    } else {
        model = p->model;
        key = ({
            register s32 value asm("r1") = sub_08219BD8(gUnk_0824DAFC);
            asm("" : "+r"(value));
            (u16) value;
        });
    }
    {
        s32 result = sub_082151E4(model, key);
        loadedModel = model;
        if (!result)
            return -1;
    }
    i = 0;
    if (i < p->drawableCount) {
        register struct Drawable **drawables asm("r8") = &p->drawables;
        do {
            register u32 offset asm("r0") = sizeof(struct Drawable) * i;
            struct Drawable *d;
            d = (struct Drawable *)((u32)*drawables + offset);
            sub_082144E4(d, loadedModel, 1);
            d->drawFlags |= 0x400;
            i++;
        } while (i < p->drawableCount);
    }
    sub_08217060(2, 12, 44);
    sub_08217074(0, 16);
    sub_0821A284(0x7BE3, p, 0);
    return 0;
}
