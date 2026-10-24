#include "global.h"

struct Part {
    u8 a, b, c;
    u8 pad[0x11];
    u8 sub[0x28];
};

struct Obj {
    u8 pad0[0x64];
    u16 w64, w66;
    u8 pad68[0x168 - 0x68];
    void *anim;
    struct Part parts[32];
};

void *AnimData_FindEntry(s32 id);
void Effect_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void SpriteDrawable_SelectTileFrame(void *, void *, u32);

static inline void **Object_GetAnimationField(struct Obj *p)
{
    return &p->anim;
}

s32 sub_08017684(struct Obj *p)
{
    s32 i;
    struct Part *e;
    u32 z;
    void **anim;

    p->anim = AnimData_FindEntry(0x1C1C);
    p->w64 = 0;
    p->w66 = 0;
    e = p->parts;
    z = 0;
    anim = Object_GetAnimationField(p);
    i = 31;
    for (; i >= 0; i--, e++) {
        void *sub = e->sub;
        e->a = z;
        e->b = z;
        e->c = z;
        Effect_Initialize(sub, *anim, 1);
        Effect_SetOrigin(sub, -4, -4);
        SpriteDrawable_SelectTileFrame(sub, *anim, 0x1E);
    }
    return 0;
}
