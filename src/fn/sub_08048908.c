#include "tether_actor.h"
#define TetherActor_RemoveAuxiliarySprite sub_08048908
void TetherActor_RemoveAuxiliarySprite(struct TetherActor *actor)
{
    u8 *active = &actor->auxiliaryActive;
    if (*active == 1) {
        Sprite_Remove(&actor->auxiliarySprite);
        *active = 0;
    }
}
