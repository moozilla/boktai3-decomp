#define SceneActorReaction_ParseOptions sub_080B7184
#include "global.h"
#include "scene_actor_reset.h"
#include "scene_actor_script.h"
static inline s32 NextValue(void)
{
    if (Script_GetPc())
        return Script_GetValue();
    return 0;
}
static inline void SetContextFlags(struct SceneActorScriptPropertiesView *p, u32 mask)
{
    p->flags6DC |= mask;
}
void sub_080B7184(struct SceneActorRegistryResetView *actor)
{
    struct SceneActorScriptPropertiesView *context = actor->context;
    s32 mode;
    s32 value;
    if (Script_SeekToKeyword('E')) {
        context->parameter6D4 = NextValue();
        context->parameter6D6 = NextValue();
    }
    if (Script_SeekToKeyword('d')) {
        mode = NextValue();
        context->parameter6D2 = NextValue();
        if (Script_GetPc())
            value = Script_GetValue();
        else
            goto mode_flags;
    } else {
        mode = 0;
        value = -1;
    }
    if (value >= 0) {
        SetContextFlags(context, 0x800);
        context->parameter702 = value;
    }
mode_flags:
    if (mode) {
        u32 flags;
        switch (mode) {
            case 1:
                flags = 0xA;
                break;
            case 2:
                flags = 0x1000;
                break;
            case 3:
                flags = 0x100A;
                break;
            default:
                return;
        }
        SetContextFlags(context, flags);
    }
}
