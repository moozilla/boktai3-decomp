#include "global.h"
#include "engine.h"
#include "render.h"

struct Registry2388ResourceHeader {
    u32 words[8];
};
struct Registry2388Animation {
    u8 pad0[0x1a];
    u8 layer;
    u8 pad1b[0x2d];
    void *palette;
    u8 pad4c[0x14];
};
struct Registry2388Display {
    u8 pad0[0x20];
    struct Registry2388Animation animations[13];
    struct Registry2388ResourceHeader resource;
};
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);

void Registry2388_InitializeAnimations(struct Registry2388Display *p)
{
    struct Registry2388ResourceHeader *resource = ScriptProc_Start(0xcb05, 0xa2db);
    if (resource) {
        struct Registry2388Animation *anim;
        void *palette;
        s32 count;
        u32 zero;
        p->resource = *resource;
        ResourceHeader_Relocate(&p->resource, resource);
        anim = p->animations;
        palette = gPaletteBankData + 0x63c0;
        zero = 0;
        count = 12;
        do {
            Widget_InitLinked(anim, &p->resource, 0, 0x10, 2, zero, 0x3c, (void *)zero);
            anim->palette = palette;
            anim->layer = 3;
            anim++;
        } while (--count >= 0);
        Anim_LoadKeyframe(&p->animations[10], &p->resource, 12, 0);
        p->animations[10].layer = 1;
        Anim_LoadKeyframe(&p->animations[9], &p->resource, 30, 0);
        p->animations[9].layer = 2;
        Anim_LoadKeyframe(&p->animations[11], &p->resource, 0, 0);
        p->animations[11].layer = 2;
        Anim_LoadKeyframe(&p->animations[12], &p->resource, 1, 0);
        p->animations[12].layer = 2;
    }
}
