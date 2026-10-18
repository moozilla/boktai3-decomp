#include "global.h"
#include "scene_actor_registry.h"
struct ActorGroupOptions {
    u8 unknown0[0xe3c];
    u8 option0, option1, option2, option3, option4, option5;
};
static inline s32 ReadOption(s32 defaultValue)
{
    s32 value;
    if (Script_GetPc())
        value = Script_GetValue();
    else
        value = defaultValue;
    return value;
}
void ActorGroup_ReadScriptOptions(struct SceneActorRegistryActor *actor)
{
    struct ActorGroupOptions *group = actor->context;
    if (Script_SeekToKeyword('O')) {
        group->option0 = ReadOption(6);
        group->option1 = ReadOption(12);
        group->option2 = ReadOption(60);
        group->option3 = ReadOption(3);
        group->option4 = ReadOption(60);
        group->option5 = ReadOption(30);
    }
}
