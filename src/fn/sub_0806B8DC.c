// MATCHING_CONCESSION: register-pinning
#include "sprite_effect_controller.h"
#define SpriteEffectController_UpdatePool sub_0806B8DC
extern u32 gUnk_030053F4, gUnk_030053F0, gUnk_02000560;
extern void *gUnk_08605B84[];
void sub_0806A60C(struct SpriteEffectControllerPool *);
static inline u8 IsUsed(struct SpriteEffectControllerPool *p, u32 index)
{
    if (p->used & (1U << index))
        return 1;
    return 0;
}
s32 SpriteEffectController_UpdatePool(struct SpriteEffectControllerPool *input)
{
    struct SpriteEffectControllerPool *p = input;
    struct SpriteEffectController *slot = p->slots;
    asm("" : "+r"(slot));
    if (p->used) {
        u32 mask = 0x400;
        if ((gUnk_030053F4 | gUnk_030053F0) & mask)
            return 0;
        {
            u32 mask = 4;
            if (gUnk_02000560 & mask)
                return 0;
        }
        sub_0806A60C(p);
        {
            s32 i = 0;
            void **table = gUnk_08605B84;
            register u8 *state asm("r6") = &p->slots[0].state;
            for (; i < 24; state += 0x7c, slot++, i++) {
                if (IsUsed(p, i))
                    sub_08249240(p, slot, table[*state]);
            }
        }
    }
    return 0;
}
