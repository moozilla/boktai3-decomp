#include "sprite_effect_controller.h"
s32 SpriteEffectController_InitializePool(struct SpriteEffectControllerPool *p)
{
    p->used = 0;
    gSpriteEffectControllerPool = p;
    return 0;
}
