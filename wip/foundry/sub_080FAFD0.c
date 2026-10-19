#define SceneActorControl_ReadHoverParameters sub_080FAFD0
#include "global.h"
#include "scene_actor_reset.h"
struct HoverControlContext {
    u8 unknown0[0xEB4];
    u16 healthThreshold;
    u8 unknownEB6[2];
    u16 counters[4];
    u16 parameters[4];
};
static inline s32 Quarter(s16 value)
{
    return value >= 0 ? value >> 2 : -(-value >> 2);
}
void sub_080FAFD0(struct SceneActorRegistryResetView *actor)
{
    struct HoverControlContext *context = actor->context;
    s32 i;
    if (Script_SeekToKeyword('P')) {
        for (i = 0; i < 4; ++i) {
            u16 *dest = (u16 *)((u32)context->parameters + ((u16)i << 1));
            *dest = Script_GetPc() ? Script_GetValue() : 0;
        }
    }
    for (i = 3; i >= 0; --i)
        context->counters[i] = 0;
    {
        s32 health = actor->health;
        s32 savedHealth = health;
        context->healthThreshold = savedHealth - Quarter(health);
    }
}
