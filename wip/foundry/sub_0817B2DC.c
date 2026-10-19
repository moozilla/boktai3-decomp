#include "global.h"
#include "scene_event_dispatch.h"
#define ActorEffect_ProcessSceneCommands sub_0817B2DC
#define gSceneEventDispatch gUnk_0200047C
struct EffectCommandActor {
    u8 unknown0[0xAC];
    u8 mode, phase, variant, unknownAF;
    u8 enter;
    u8 unknownB1[0x13];
    u32 timer;
    u8 unknownC8[0x4C];
    u8 direction;
    u8 unknown115[3];
    struct SceneEventReceiver receiver;
};
void sub_0817B2DC(struct EffectCommandActor *actor)
{
    struct SceneEventReceiver *receiver = &actor->receiver;
    s32 i;
    for (i = 0; i < actor->receiver.count[gUnk_0200047C->phase]; i++) {
        struct SceneEventCommand *command = receiver->pending[gUnk_0200047C->phase][i];
        sub_08020D54(receiver, command);
        switch (command->kind) {
        case 1:
            actor->mode = 0;
            actor->phase = 0;
            actor->variant = 0;
            actor->timer = 0;
            actor->enter = 1;
            break;
        case 5:
            actor->direction = (((u8 *)&command->value)[0] + 5) << 5;
            actor->mode = 2;
            actor->phase = 0;
            actor->variant = 1;
            actor->timer = 0;
            actor->enter = 1;
            break;
        case 27:
            actor->mode = 2;
            actor->phase = 0;
            actor->variant = 2;
            actor->timer = 0;
            actor->enter = 1;
            break;
        case 28:
            actor->mode = 2;
            actor->phase = 0;
            actor->variant = 3;
            actor->timer = 0;
            actor->enter = 1;
            break;
        }
    }
}
