#include "global.h"
#define CallbackEffectPool_Destroy sub_0819A414
#define Callback_Invoke sub_0824923C
struct CallbackEffect { u32 active; void *destroy; };
struct CallbackEffectPool { u8 unknown[0x28]; };
void sub_0824923C(void *, void *);
s32 sub_0819A414(struct CallbackEffectPool *pool)
{
    s32 i;
    for (i = 0; i <= 15; i++) {
        struct CallbackEffect *effect = (struct CallbackEffect *)(i * 0xd4 + (u32)pool + 0x28);
        effect->active = 0;
        if (effect->destroy) {
            sub_0824923C(effect, effect->destroy);
            effect->destroy = 0;
        }
    }
    for (i = 0; i <= 7; i++) {
        struct CallbackEffect *effect = (struct CallbackEffect *)(i * 0xb8 + (u32)pool + 0xd68);
        effect->active = 0;
        if (effect->destroy) {
            sub_0824923C(effect, effect->destroy);
            effect->destroy = 0;
        }
    }
    return 0;
}
