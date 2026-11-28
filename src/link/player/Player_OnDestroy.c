#include "global.h"
#include "engine/ram.h"
#include "player/player.h"
void SceneActorContacts_RemoveNode(void *);
void Anim_Remove(void *);
void Sprite_Remove(void *);
void Collider_Remove(void *);
void SceneBody_Unregister(void *);
void LinkPlayer_RemoveAuxiliaryResources(struct Player *);
void Player_StopLocalActionSounds(struct Player *);
s32 Player_OnDestroy(struct Player *p)
{
    SceneActorContacts_RemoveNode(p->contactListNode);
    Anim_Remove(&p->animation);
    Sprite_Remove(&p->secondaryDrawFlags);
    Collider_Remove(&p->collider);
    SceneBody_Unregister(&p->positionalEffectGroup);
    LinkPlayer_RemoveAuxiliaryResources(p);
    Player_StopLocalActionSounds(p);
    gPlayers[p->slot] = NULL;
    gPlayerCount--;
    return 0;
}
