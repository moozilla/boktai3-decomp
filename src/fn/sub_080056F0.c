#include "global.h"
#include "scene_object.h"

extern u8 *gUnk_030042E4;
void *sub_08219C40(u32);
void sub_08219DD8(void *, u32);

/* Allocate the object pool (default 8 objects) and initialize the palette-fade state. */
s32 sub_080056F0(struct SceneObjectManager *p, void *unused)
{
    void *m;
    u8 *b;
    gUnk_02000024 = p;
    if (p->count == 0)
        p->count = 8;
    m = sub_08219C40(p->count * sizeof(struct SceneObject));
    p->objects = m;
    if (m == 0)
        return -1;
    sub_08219DD8(m, p->count * sizeof(struct SceneObject));
    p->frame = 0;
    p->fading = 0;
    p->descending = 0;
    p->fade = 0;
    b = gUnk_030042E4;
    p->fromA = (u32)(b + 0x80);
    p->fromB = (u32)(b + 0xa0);
    p->toA = (u32)(b + 0xc0);
    p->toB = (u32)(b + 0xe0);
    CpuSet((void *)p->paletteA[0], &p->paletteA[0], 0x04000008);
    CpuSet((void *)p->paletteA[1], p->paletteB, 0x04000008);
    return 0;
}
