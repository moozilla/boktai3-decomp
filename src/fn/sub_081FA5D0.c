#include "global.h"
struct SceneResourceSprite {
    u8 pad0[8];
    u32 flags;
};
struct SceneResourceObject {
    u8 pad0[0x32C];
    u8 resource[32];
    struct SceneResourceSprite sprite;
};
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
u32 SceneResource_InitializeHiddenSprite(struct SceneResourceObject *p)
{
    void *source = ScriptProc_Start(0xCB05, 0xFB81);
    void *resource = p->resource;
    ResourceHeader_Relocate(resource, source);
    sub_0821983C(&p->sprite, resource, 0, 0, 2, 0, 0, 0);
    p->sprite.flags |= 1;
}
