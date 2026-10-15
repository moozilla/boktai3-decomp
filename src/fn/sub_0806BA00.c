#include "sprite_effect_controller.h"
s32 SpriteEffectController_InitializePool(struct SpriteEffectControllerPool *);
s32 SpriteEffectController_UpdatePool(struct SpriteEffectControllerPool *);
s32 SpriteEffectController_DestroyPool(struct SpriteEffectControllerPool *);
struct SpriteEffectControllerPool *SpriteEffectController_EnsurePool(void)
{
    struct SpriteEffectControllerPool *p;
    if (gUnk_02000178)
        return gUnk_02000178;
    p = Task_Create(10, sizeof(*p));
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))SpriteEffectController_UpdatePool,
                          (void (*)(void *))SpriteEffectController_DestroyPool);
        if (SpriteEffectController_InitializePool(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
