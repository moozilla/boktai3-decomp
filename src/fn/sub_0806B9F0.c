#include "sprite_effect_controller.h"
#define SpriteEffectController_InitializePool sub_0806B9F0
s32 SpriteEffectController_InitializePool(struct SpriteEffectControllerPool *p)
{
    p->used = 0;
    gUnk_02000178 = p;
    return 0;
}
