#include "global.h"
#include "ram.h"
void SceneActorContacts_RemoveNode(void *);
void Anim_Remove(void *);
void Sprite_Remove(void *);
void Collider_Remove(void *);
void SceneBody_Unregister(void *);
void LinkPlayer_RemoveAuxiliaryResources(void *);
void Player_StopLocalActionSounds(void *);
extern u32 gUnk_02000580[];
s32 Player_OnDestroy(u8 *p)
{
    SceneActorContacts_RemoveNode(p + 0x2E8);
    Anim_Remove(p + 0x8C);
    Sprite_Remove(p + 0x14C);
    Collider_Remove(p + 0x230);
    SceneBody_Unregister(p + 0x24);
    LinkPlayer_RemoveAuxiliaryResources(p);
    Player_StopLocalActionSounds(p);
    gUnk_02000580[*(u32 *)(p + 0x18)] = 0;
    gPlayerCount--;
    return 0;
}
