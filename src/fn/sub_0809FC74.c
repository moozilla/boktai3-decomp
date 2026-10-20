// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_chase.h"

#ifdef NONMATCHING
// fuzzy: 91%
// Remaining differences: register allocation of pool/offset constants.

extern const u16 gUnk_0203B400[];
struct CrowWanderContext {
    u8 unknown0[0x3D4];
    u32 flags3D4;
    u8 unknown3D8[0x15];
    u8 wanderTicks;
};
static inline u32 Random(void)
{
    u32 *p = &gUnk_03005308;
    u32 i = (*p + 1) & 1023;
    *p = i;
    i *= 2;
    return *(const u16 *)((u32)gUnk_0203B400 + i);
}

inline static u8 Test2(int a1, u32 a0)
{
    return Chase_Test(a0, a1);
}

#define Crow_UpdateWanderTimer sub_0809FC74
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
    if (!Test2(4, context->flags3D4)) {
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
#else
INCLUDE_ASM("asm/nonmatching", sub_0809FC74);
#endif
