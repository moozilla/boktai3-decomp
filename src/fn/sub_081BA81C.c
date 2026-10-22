#include "global.h"
#include "engine.h"

struct Registry3F74ResourceHeader {
    u32 words[8];
};
struct Registry3F74Animation {
    u8 pad0[0x1a];
    u8 layer;
    u8 pad1b[0x45];
};
struct Registry3F74Display {
    u8 pad0[0x20];
    struct Registry3F74Animation animations[5];
    u8 pad200[0x200 - 0x20 - 5 * 0x60];
    struct Registry3F74ResourceHeader resource;
};
void Widget_InitLinked(void *, void *, int, int, int, int, int, void *);

void Registry3F74_InitAnimations(struct Registry3F74Display *p)
{
    struct Registry3F74ResourceHeader *resource = ScriptProc_Start(0xcb05, 0xa2db);
    if (resource) {
        struct Registry3F74Animation *anim;
        s32 count;
        p->resource = *resource;
        ResourceHeader_Relocate(&p->resource, resource);
        anim = p->animations;
        for (count = 0; count < 5; count++) {
            Widget_InitLinked(anim, &p->resource, 0, 0x10, 2, 0, 0x3c, 0);
            anim->layer = 3;
            anim++;
        }
        Anim_LoadKeyframe(&p->animations[2], &p->resource, 0x26, 0);
        p->animations[2].layer = 1;
        Anim_LoadKeyframe(&p->animations[0], &p->resource, 0x27, 0);
        Anim_LoadKeyframe(&p->animations[1], &p->resource, 0x2b, 0);
        p->animations[1].layer = 2;
        Anim_LoadKeyframe(&p->animations[3], &p->resource, 0, 0);
        p->animations[3].layer = 2;
        Anim_SetKeyframes(&p->animations[4], &p->resource, 4, 1, 0);
        Anim_Step(&p->animations[4], &p->resource);
    }
}
