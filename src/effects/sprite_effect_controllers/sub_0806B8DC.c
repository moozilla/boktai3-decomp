#include "engine/shared/sprite_effect_controller.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "engine/shared/player.h"
#include "engine/shared/engine_pause.h"
typedef void (*SpriteEffectStateCallback)(struct SpriteEffectControllerPool *,
                                          struct SpriteEffectController *);
extern SpriteEffectStateCallback gUnk_08605B84[];
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
        if (Engine_PauseBits(0x400))
            return 0;
        {
            u32 mask = 4;
            if (gPlayerSlotStatus[0] & mask)
                return 0;
        }
        SpriteEffectController_QueueColliders(p);
        {
            s32 i = 0;
            SpriteEffectStateCallback *table = gUnk_08605B84;
            for (; i < 24; i++) {
                if (IsUsed(p, i))
                    table[slots[i].state](p, &slots[i]);
            }
        }
    }
    return 0;
}
