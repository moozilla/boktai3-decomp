#include "sprite_effect_controller.h"
s32 SpriteEffectController_InitializePool(struct SpriteEffectControllerPool *p)
{
    p->used = 0;
    gUnk_02000178 = p;
    return 0;
}
