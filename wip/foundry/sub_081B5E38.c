#define PairedSceneActor_Destroy sub_081B5E38
#define gPairedSceneActor gUnk_02000254
#include "global.h"
struct ScenePart { u8 unknown0[4]; u8 active; u8 unknown5[0x217]; };
struct PairedSceneActor { u8 task[0x18]; u8 animation[0x80]; u8 entries[2][0x21c]; };
void Anim_Remove(void *);
void Collider_Remove(void *);
void Sprite_Remove(void *);
void sub_08013B74(void *);
void sub_0815F738(void *);
extern void *gUnk_02000254;
void sub_081B5E38(struct PairedSceneActor *p)
{
    s32 i;
    struct ScenePart *sprite, *collider, *route, *effect;
    Anim_Remove(p->animation);
    effect = (struct ScenePart *)((u8 *)p + 0x2a4);
    route = (struct ScenePart *)((u8 *)p + 0x238);
    collider = (struct ScenePart *)((u8 *)p + 0x1a0);
    sprite = (struct ScenePart *)p->entries[0];
    i = 1;
    do {
        Collider_Remove(collider);
        if (sprite->active) Sprite_Remove(sprite);
        sub_08013B74(route);
        sub_0815F738(effect);
        effect++;
        route++;
        collider++;
        sprite++;
    } while (--i >= 0);
    gUnk_02000254 = 0;
}
