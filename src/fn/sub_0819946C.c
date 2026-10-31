#include "global.h"
#include "game_state.h"
#include "render.h"
#include "scene_actor_registry.h"

struct Message {
    u32 id : 16;
    u32 reserved : 8;
    u32 count : 8;
    s16 *values;
};
void MessageQueue_PostMessage(struct Message *);

static inline s32 ScaleProjection(s32 v)
{
    return v >= 0 ? v >> 5 : -((-v) >> 5);
}

#define SET_PROJECTED_EFFECT_SIDE(position, side)                                                  \
    do {                                                                                           \
        struct ContactPosition projected;                                                          \
        s32 rightSide;                                                                             \
        struct ContactPosition *output;                                                            \
        const struct ContactPosition *input;                                                       \
        s32 x, z, sum, screenY, height, depth;                                                     \
        output = &projected;                                                                       \
        input = (position);                                                                        \
        x = input->x;                                                                              \
        z = input->z;                                                                              \
        output->x = ScaleProjection(3 * (x - z));                                                  \
        sum = ScaleProjection(3 * (x + z));                                                        \
        height = ScaleProjection(3 * input->y);                                                    \
        screenY = sum - height;                                                                    \
        rightSide = 0;                                                                             \
        depth = sum + height;                                                                      \
        output->x = output->x - gCameraOrigin[0] + 120;                                            \
        output->y = screenY - gCameraOrigin[1] + 80;                                               \
        output->z = depth - gCameraOrigin[2];                                                      \
        if (projected.x > 120)                                                                     \
            (side) = rightSide;                                                                    \
        else                                                                                       \
            (side) = 1;                                                                            \
    } while (0)

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

#ifdef NONMATCHING
struct PackedPosition {
    u32 x : 16, y : 16, z : 16, w : 16;
};
#endif

#ifdef NONMATCHING
// fuzzy: 97.0%
void SceneEffectQueue_PostTypeThreeMessage(s32 value)
{
    s16 values[3];
    struct Message message;
    struct PackedPosition in;
    struct ContactPosition out;
    s32 sum, height, screenY, depth, x, z, zero;
    struct ContactPosition *input, *output;
    message.id = 0x382f;
    message.count = 3;
    message.values = values;
    in.x = (u16)gGameState->positionX;
    in.y = (u16)gGameState->positionY;
    in.z = (u16)gGameState->positionZ;
    output = &out;
    input = (struct ContactPosition *)&in;
    x = input->x;
    z = input->z;
    output->x = ScaleProjection(3 * (x - z));
    sum = ScaleProjection(3 * (x + z));
    height = ScaleProjection(3 * input->y);
    screenY = sum - height;
    zero = 0;
    depth = sum + height;
    output->x = output->x - (*(struct ContactPosition *)gCameraOrigin).x + 120;
    output->y = screenY - (*(struct ContactPosition *)gCameraOrigin).y + 80;
    output->z = depth - (*(struct ContactPosition *)gCameraOrigin).z;
    if (out.x > 120)
        values[1] = zero;
    else
        values[1] = 1;
    values[0] = (u8)(zero + 3);
    values[2] = value;
    MessageQueue_PostMessage(&message);
}
#else
INCLUDE_ASM("asm/nonmatching", SceneEffectQueue_PostTypeThreeMessage);
#endif
