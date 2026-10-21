// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_chase.h"

#ifdef NONMATCHING
// fuzzy: 98.7%
// Remaining differences: 0x3ED offset constant register (r2 vs r3) in the wander-ticks store.

extern const u16 gUnk_0203B400[];
struct CrowWanderContext {
    u8 unknown0[0x3D4];
    u32 flags3D4;
    u8 unknown3D8[0x15];
    u8 wanderTicks;
};
#define AT(base, off, rc, ra)                                                                      \
    ({                                                                                             \
        register u32 _o asm(rc) = (off);                                                           \
        register u8 *_a asm(ra);                                                                   \
        asm("" : "+r"(_o));                                                                        \
        _a = (u8 *)(base) + _o;                                                                    \
        _a;                                                                                        \
    })
static inline u32 Random(void)
{
    u32 *p = &gUnk_03005308;
    u32 i = (*p + 1) & 1023;
    *p = i;
    i *= 2;
    {
        register u32 tab asm("r2") = (u32)gUnk_0203B400;
        asm("" : "+r"(tab));
        return *(const u16 *)(i + tab);
    }
}

inline static u8 Test2(int a1, u32 a0)
{
    return Chase_Test(a0, a1);
}

void Crow_UpdateWanderTimer(struct SceneActorRegistryResetView *actor)
{
    struct CrowWanderContext *context = actor->context;
    if (Chase_TakeArrived(actor)) {
        u32 mask = 0x7F;
        u32 value;
        Chase_SetCtl(actor, actor->savedTarget, 0);
        value = Random();
        context->wanderTicks = (mask & value) + 0x14;
    }
    {
        register u32 four asm("r1") = 4;
        asm("" : "+r"(four));
        if (!Test2(four, *(u32 *)AT(context, 0x3D4, "r2", "r0"))) {
            {
                u8 result = Callback_InvokeActor(actor, actor->update270);
                if (!result) {
                    if ((s32)actor->counter2AC > context->wanderTicks && actor->field2B3) {
                        Chase_SetBehavior(actor, 2);
                    } else {
                        actor->counter2AC++;
                    }
                }
            }
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Crow_UpdateWanderTimer);
#endif
