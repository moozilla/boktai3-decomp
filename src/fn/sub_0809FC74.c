#include "global.h"
#include "scene_actor_chase.h"

struct CrowWanderContext {
    u8 unknown0[0x3D4];
    u32 flags3D4;
    u8 unknown3D8[0x15];
    u8 wanderTicks;
};
static inline u32 Random(void)
{
    u32 *indexPtr = &gRandomIndex;
    u32 i = (*indexPtr + 1) & 1023;
    *indexPtr = i;
    i *= 2;
    {
        u32 tab = RANDOM_TABLE_ADDR;
        return *(const u16 *)(i + tab);
    }
}

static inline u8 Crow_TestFlags(u32 mask, u32 flags)
{
    return Chase_Test(flags, mask);
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
        u32 four = 4;
        if (!Crow_TestFlags(four, context->flags3D4)) {
            u8 result = Callback_InvokeActor(actor, actor->contactQuery.opaque);
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
