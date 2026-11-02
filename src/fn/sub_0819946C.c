#include "global.h"
#include "game_state.h"
#include "scene_actor_registry.h"
#include "scene_effect_projection.h"

struct Message {
    u32 id : 16;
    u32 reserved : 8;
    u32 count : 8;
    s16 *values;
};
void MessageQueue_PostMessage(struct Message *);

static inline void PostSceneEffect(s32 value, u32 type)
{
    s16 values[3];
    struct Message message;
    union SceneActorRegistryPackedPosition in;

    message.id = 0x382f;
    message.count = 3;
    message.values = values;
    in.packed.x = (u16)gGameState->positionX;
    in.packed.y = (u16)gGameState->positionY;
    in.packed.z = (u16)gGameState->positionZ;
    SET_PROJECTED_EFFECT_SIDE(&in.position, values[1]);

    values[0] = type;
    values[2] = value;
    MessageQueue_PostMessage(&message);
}

void SceneEffectQueue_PostTypeZeroMessage(s32 value)
{
    PostSceneEffect(value, 0);
}

void SceneEffectQueue_PostTypeOneMessage(s32 value)
{
    PostSceneEffect(value, 1);
}

void SceneEffectQueue_PostTypeThreeMessage(s32 value)
{
    PostSceneEffect(value, 3);
}