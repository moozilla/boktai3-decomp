#include "sprite_effect_controller.h"
#include "engine.h"
#include "ram.h"
extern u32 gUnk_02000560;
extern void *gUnk_08605B84[];
void SpriteEffectController_QueueColliders(struct SpriteEffectControllerPool *);
static inline u8 IsUsed(struct SpriteEffectControllerPool *p, u32 index)
{
    if (p->used & (1U << index))
        return 1;
    return 0;
}
s32 SpriteEffectController_UpdatePool(struct SpriteEffectControllerPool *input)
{
    struct SpriteEffectControllerPool *p = input;
    struct SpriteEffectController *slots = p->slots;
    if (p->used) {
        u32 mask = 0x400;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            return 0;
        {
            u32 mask = 4;
            if (gUnk_02000560 & mask)
                return 0;
        }
        SpriteEffectController_QueueColliders(p);
        {
            s32 i = 0;
            void **table = gUnk_08605B84;
            for (; i < 24; i++) {
                if (IsUsed(p, i))
                    sub_08249240(p, &slots[i], table[slots[i].state]);
            }
        }
    }
    return 0;
}
