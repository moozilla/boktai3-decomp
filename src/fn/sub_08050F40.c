#include "orbit_background.h"
static inline s32 scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
s32 OrbitBackground_Initialize(struct OrbitBackground *background, u32 id, u32 player, u32 enabled)
{
    background->id = id;
    background->playerIndex = player;
    background->enabled = enabled;
    OrbitBackground_LoadResource(background);
    OrbitBackground_ResetOrbit(background);
    background->position.x =
        gUnk_02000580[background->playerIndex]->position.x -
        scale(gUnk_086149C4[(background->angle + 64) & 255] * background->radius);
    {
        struct OrbitBackgroundPlayer *actor = gUnk_02000580[background->playerIndex];
        background->position.y = (((128 - actor->heightScale) * 150) >> 7) + (u16)actor->position.y;
    }
    background->position.z = gUnk_02000580[background->playerIndex]->position.z -
                             scale(gUnk_086149C4[background->angle & 255] * background->radius);
    OrbitBackground_Tick(background);
    gOrbitBackground = background;
    return 0;
}
