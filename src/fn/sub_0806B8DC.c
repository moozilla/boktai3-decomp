#include "sprite_effect_controller.h"
#include "engine.h"
extern u32 gUnk_030053F0, gUnk_02000560;
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
    struct SpriteEffectController *slot;
    do {
        do {
            slot = p->slots;
        } while (0);
    } while (0);
    if (p->used) {
        u32 mask = 0x400;
        if ((gEngineFlags | gUnk_030053F0) & mask) {
            return 0;
        }
        {
            u32 mask = 4;
            if (gUnk_02000560 & mask) {
                return 0;
            }
        }
        SpriteEffectController_QueueColliders(p);
        {
            s32 i = 0;
            void **table = gUnk_08605B84;
            u8 *state = &p->slots[0].state;
            for (; i < 24; state += 0x7c, slot++, i++) {
                if (IsUsed(p, i)) {
                    sub_08249240(p, slot, table[*state]);
                }
            }
        }
    }
    return 0;
}
