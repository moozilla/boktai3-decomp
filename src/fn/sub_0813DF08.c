#include "global.h"

void SceneActorEffects_DestroyGroup(void *);
void Shadow_Destroy(void *);
void Effect_Remove(void *);
void ConvergingEffects_RemoveSprites(void *);
void DirectionalSpriteEffect_RemoveSprite(void *);

void Player_DestroyEffects(u8 *p)
{
    SceneActorEffects_DestroyGroup(p + 0x5B8);
    Shadow_Destroy(p + 0x700);
    Effect_Remove(p + 0x748);
    Effect_Remove(p + 0x778);
    Effect_Remove(p + 0x7A0);
    Effect_Remove(p + 0x7D0);
    ConvergingEffects_RemoveSprites(p);
    DirectionalSpriteEffect_RemoveSprite(p);
}
