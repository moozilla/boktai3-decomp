#include "sprite_effect_controller.h"
#define SpriteEffectController_EnsurePool sub_0806BA00
s32 sub_0806B9F0(struct SpriteEffectControllerPool *);
s32 sub_0806B8DC(struct SpriteEffectControllerPool *);
s32 sub_0806B96C(struct SpriteEffectControllerPool *);
struct SpriteEffectControllerPool *SpriteEffectController_EnsurePool(void)
{
    struct SpriteEffectControllerPool *p;
    if (gUnk_02000178)
        return gUnk_02000178;
    p = Task_Create(10, sizeof(*p));
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))sub_0806B8DC, (void (*)(void *))sub_0806B96C);
        if (sub_0806B9F0(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
