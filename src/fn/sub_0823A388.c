#include "global.h"
void SceneActorEffects_DestroyGroup(u8 *);
void Shadow_Destroy(u8 *);
void Effect_Remove(u8 *);
void Player_DestroySixParticleEffects(u8 *);
void Player_DestroyConvergingParticles(u8 *);
void Player_DestroyDirectionalEffect(u8 *);
void LinkPlayer_RemoveAuxiliaryResources(u8 *p)
{
    SceneActorEffects_DestroyGroup(p + 0x5b8);
    Shadow_Destroy(p + 0x700);
    Effect_Remove(p + 0x748);
    Effect_Remove(p + 0x7d0);
    Player_DestroySixParticleEffects(p);
    Player_DestroyConvergingParticles(p);
    Player_DestroyDirectionalEffect(p);
}
