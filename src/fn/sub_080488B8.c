#include "tether_actor.h"
#define TetherActor_CreateAuxiliarySprite sub_080488B8
void TetherActor_CreateAuxiliarySprite(struct TetherActor *actor)
{
    u8 *active = &actor->auxiliaryActive;
    if (!*active) {
        struct ContactSwarmGraphics *graphics = &actor->auxiliaryGraphics;
        struct ContactSwarmSprite *sprite;
        AnimData_Load(graphics, 0x1C1B);
        sprite = &actor->auxiliarySprite;
        Sprite_Init(sprite, graphics, 512);
        sub_08214584(sprite, -8, -8);
        sprite->frame = 6;
        *active = 1;
    }
}
