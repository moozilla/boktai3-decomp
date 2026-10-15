#include "global.h"

void Sprite_Remove(void *);
void Anim_Remove(void *);
void Heap_Free(void *);
void sub_08013B74(void *);
void sub_08003C10(void *);
void sub_08225938(void *);
void Collider_Remove(void *);
void SceneActorRegistry_UnlinkActor(void *);

s32 sub_080A4AA0(u8 *p)
{
    u8 *gfx = *(u8 **)(p + 4);
    if (!p[8])
        Sprite_Remove(gfx);
    else
        Anim_Remove(gfx + 0x20);
    Heap_Free(gfx);
    if (p[0x360])
        sub_08013B74(p + 0x360);
    sub_08003C10(p + 0x50);
    sub_08225938(p + 0x50);
    Collider_Remove(p + 0x134);
    SceneActorRegistry_UnlinkActor(p);
    return 0;
}
