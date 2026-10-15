#include "global.h"
void SceneActorContacts_RemoveNode(void *);
void Anim_Remove(void *);
void Sprite_Remove(void *);
void Collider_Remove(void *);
void sub_08225938(void *);
void sub_0823A388(void *);
void sub_082384C8(void *);
extern u32 gUnk_02000580[];
extern u16 gUnk_02000544;
s32 sub_082439EC(u8 *p)
{
    SceneActorContacts_RemoveNode(p + 0x2E8);
    Anim_Remove(p + 0x8C);
    Sprite_Remove(p + 0x14C);
    Collider_Remove(p + 0x230);
    sub_08225938(p + 0x24);
    sub_0823A388(p);
    sub_082384C8(p);
    gUnk_02000580[*(u32 *)(p + 0x18)] = 0;
    gUnk_02000544--;
    return 0;
}
