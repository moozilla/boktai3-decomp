#define Player_Destroy       sub_0815F224
#define Player_RemoveEffects Player_DestroyEffects
#define gPlayerCount         gUnk_02000544
#define gPlayerCueTimer      gUnk_02000518
#define gPlayerCueActive     gUnk_02000528
#include "global.h"
#include "player.h"
void SceneActorContacts_RemoveNode(void *);
void Anim_Remove(void *);
void Sprite_Remove(void *);
void Collider_Remove(void *);
void SceneBody_Unregister(void *);
void Player_DestroyEffects(void *);
void Player_UnregisterSceneMessageReceiver(void *);
void Player_StopActionSounds(void *);
extern u32 gUnk_0200054C;
extern u16 gUnk_02000594, gUnk_02000544, gUnk_02000590, gUnk_02000570, gUnk_02000528, gUnk_02000518;
s32 sub_0815F224(struct Player *p)
{
    SceneActorContacts_RemoveNode(&p->contactListNode);
    Anim_Remove(&p->animation);
    Anim_Remove(&p->secondaryAnimation);
    Sprite_Remove(p->auxiliarySprite);
    Collider_Remove(&p->collider);
    SceneBody_Unregister(&p->positionalEffectGroup);
    Player_DestroyEffects(p);
    Player_UnregisterSceneMessageReceiver(p);
    Player_StopActionSounds(p);
    gUnk_0200054C = 0;
    gUnk_02000594 = 0;
    gUnk_02000580[p->movementRequestBlocked] = 0;
    gUnk_02000544--;
    gUnk_02000590 = 0;
    gUnk_02000570 = 0;
    gUnk_02000528 = 0;
    gUnk_02000518 = 0;
    return 0;
}
