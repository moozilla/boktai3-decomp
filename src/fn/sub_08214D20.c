#include "global.h"
#include "render.h"
#include "ram.h"

// Sprite list cull/draw, unscaled variant of Sprite_RenderScaledList.
// The nested do/while(0) around the list-head load (likely a macro in the
// original) is what gives p its register.
struct Vec {
    s16 x, y, z;
};
struct SpriteResource {
    u8 pad[8];
    u8 width, height;
    s8 offsetX, offsetY;
};
struct Sprite {
    u32 flags;
    u8 pad4[4];
    s8 scaleX, scaleY;
    u8 padA[2];
    struct SpriteResource *resource;
    u8 pad10[12];
    struct Vec pos;
    u32 previous;
    struct Sprite *next;
};
extern struct Sprite *gUnk_030042C0[];
void sub_08214600(struct Sprite *, s32, s32, s32);
static inline s32 div8(s32 v)
{
    return v >= 0 ? v >> 3 : -((-v) >> 3);
}
void sub_08214D20(void)
{
    struct Sprite *p;
    s32 z;
    u32 width, height;
    s32 left, top;
    u32 parity = gSceneEffectPalettePhase & 1;
    u32 skip = 0x401;
    if (parity) {
        skip = 0x201;
    }
    do {
        do {
            p = gUnk_030042C0[gRenderListIndex];
            if (!p) {
                return;
            }
        } while (0);
    } while (0);
    do {
        s32 x;
        s32 y;
        struct SpriteResource *resource;
        if (p->flags & skip) {
            goto next;
        }
        resource = p->resource;
        x = div8(p->pos.x - gCamera.position.x) + 120;
        y = div8(p->pos.y - gCamera.position.y) + 80;
        z = p->pos.z - gCamera.position.z;
        if (!(p->flags & 64)) {
            if (p->flags & 4) {
                left = x + resource->offsetX - resource->width;
                width = resource->width;
            } else {
                left = x - resource->offsetX;
                width = resource->width;
            }
            if (p->flags & 8) {
                top = y + resource->offsetY - resource->height;
                height = resource->height;
            } else {
                top = y - resource->offsetY;
                height = resource->height;
            }
            if (left > 240 || top > 160 || left + (s32)width < 0 || top + (s32)height < 0) {
                goto next;
            }
        }
        sub_08214600(p, x, y, z);
    next:
        p = p->next;
    } while (p);
}
