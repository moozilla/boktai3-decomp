#include "global.h"
struct ResourceSpriteEntry {
    s32 values[4];
};
struct ResourceSpriteHeader {
    u32 words[8];
};
struct ResourceSprite {
    u8 pad0[0x18];
    u8 sprite[0x60];
    struct ResourceSpriteHeader resource;
    u8 pad98[8];
    u16 selection;
    s16 entryCount;
    struct ResourceSpriteEntry entries[16];
};
void *ScriptProc_Start(u16, u16);
s32 ResourceHeader_Relocate(void *, void *);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
void ResourceSprite_Initialize(struct ResourceSprite *object, u32 palette)
{
    struct ResourceSpriteHeader *resource = ScriptProc_Start(0xCB05, 0xA2DB);
    if (resource) {
        object->resource = *resource;
        ResourceHeader_Relocate(&object->resource, resource);
        sub_0821983C(object->sprite, &object->resource, 2, 0x30, (u8)palette, 0, 0x3C, 0);
        object->entryCount = 0;
        object->selection = 0xFFFF;
    }
}
