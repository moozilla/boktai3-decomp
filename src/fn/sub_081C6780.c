#include "state_actor.h"
#include "scene_event_dispatch.h"
#include "monster_particle_random.h"
#include "input.h"

struct DirectionCommand {
    u8 header[8];
    struct StateActorPos target;
};
s32 Math_Atan2(s32, s32);
void sub_0802EA90(void);

static inline s32 TargetAngle(struct StateActorPos *target, struct StateActorPos *position)
{
    return Math_Atan2(target->x - position->x, target->z - position->z);
}

void StateActor_ProcessDirectionMessages(struct StateActor *actor)
{
    u8 enter = 1;
    struct SceneEventReceiver *receiver = (void *)actor->receiver;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase])
        do {
            struct SceneEventDispatchState *event = gSceneEventDispatchState;
            struct SceneEventCommand *command = receiver->pending[event->phase][i];
            switch (command->kind) {
                case 4: {
                    struct DirectionCommand *message = (void *)command;
                    actor->target.x = message->target.x;
                    actor->target.y = message->target.y;
                    actor->target.z = message->target.z;
                    actor->targetAngle = TargetAngle(&actor->target, &actor->position);
                    {
                        actor->unk2[0] = 0;
                        actor->unk2[1] = 0;
                        actor->state = 2;
                        actor->timer = 0;
                        actor->enterCommand = enter;
                    }
                    break;
                }
                case 3:
                    actor->targetAngle = (*(u8 *)&command->value + 5) << 5;
                    {
                        actor->unk2[0] = 0;
                        actor->unk2[1] = 0;
                        actor->state = 3;
                        actor->timer = 0;
                        actor->enterCommand = 1;
                    }
                    break;
            }
            SceneMessages_SetPendingTrigger(receiver, command);
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    if (gPadState[1] & 2) {
        const u16 *table = gRandomTable;
        s32 x = ((s32)ParticleRandom_ReadDirectSample(table) >> 4) & 1023;
        s32 z = ((s32)ParticleRandom_ReadDirectSample(table) >> 4) & 1023;
        struct StateActorPos *target = &actor->target;
        {
            u32 base = 1024;
            u32 height;
            x += base;
            height = 256;
            z += base;
            target->x = x;
            target->y = height;
            target->z = z;
        }
        actor->targetAngle = TargetAngle(target, &actor->position);
        {
            actor->unk2[0] = 0;
            actor->unk2[1] = 0;
            actor->state = 2;
            actor->timer = 0;
            actor->enterCommand = 1;
        }
        {
            void (*update)(void) = sub_0802EA90;
            actor->prevState = 3;
            actor->stateCallback = update;
        }
        actor->stateFrame = 0;
        actor->enter = 1;
        actor->phase = 0;
    }
}
