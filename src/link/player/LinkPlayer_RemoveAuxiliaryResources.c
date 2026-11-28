#include "global.h"
#include "player/player.h"

void SceneActorEffects_DestroyGroup(void *);
void Shadow_Destroy(void *);
void Effect_Remove(void *);
void Player_DestroySixParticleEffects(struct Player *);
void Player_DestroyConvergingParticles(struct Player *);
void Player_DestroyDirectionalEffect(struct Player *);

void LinkPlayer_RemoveAuxiliaryResources(struct Player *p)
{
    SceneActorEffects_DestroyGroup(&p->effectGroup);
    Shadow_Destroy(p->shadow);
    Effect_Remove(p->visualEffects748);
    Effect_Remove(p->visualEffects7D0);
    Player_DestroySixParticleEffects(p);
    Player_DestroyConvergingParticles(p);
    Player_DestroyDirectionalEffect(p);
}
