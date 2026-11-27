#include "effects/screen/orbit_background.h"
#include "bosses/vanargand/vanargand_projectiles.h"
s32 OrbitBackground_Initialize(struct OrbitBackground *background, u32 id, u32 player, u32 enabled)
{
    background->id = id;
    background->playerIndex = player;
    background->enabled = enabled;
    OrbitBackground_LoadResource(background);
    OrbitBackground_ResetOrbit(background);
    background->position.x =
        gPlayers[background->playerIndex]->position.x -
        VanargandProjectile_Scale(gUnk_086149C4[(background->angle + 64) & 255] * background->radius);
    {
        struct Player *actor = gPlayers[background->playerIndex];
        background->position.y = (((128 - actor->visualSlowdownTimer) * 150) >> 7) + (u16)actor->position.y;
    }
    background->position.z = gPlayers[background->playerIndex]->position.z -
                             VanargandProjectile_Scale(gUnk_086149C4[background->angle & 255] * background->radius);
    OrbitBackground_Tick(background);
    gOrbitBackground = background;
    return 0;
}
