#include "global.h"
#include "game_state.h"
#include "render.h"
#include "contact_position.h"

struct PackedPosition {
    u32 x : 16, y : 16, z : 16, w : 16;
};
struct Message {
    u32 id : 16;
    u32 reserved : 8;
    u32 count : 8;
    s16 *values;
};
void MessageQueue_PostMessage(struct Message *);

static inline s32 ScaleProjection(s32 v)
{
    s32 r;
    if (v >= 0)
        r = v >> 5;
    else
        r = -((-v) >> 5);
    return r;
}
#ifdef NONMATCHING
// fuzzy: 97.0%
// Tentative: post a type-zero scene-effect message for the player's screen side.
void SceneEffectQueue_PostTypeZeroMessage(s32 value)
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
    values[0] = (u8)zero;
    values[2] = value;
    MessageQueue_PostMessage(&message);
}
/* Recovered adjacent routine: same message payload, effect type one. */
void sub_08199578(s32 value)
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
    values[0] = (u8)(zero + 1);
    values[2] = value;
    MessageQueue_PostMessage(&message);
}
/* Recovered adjacent routine: same message payload, effect type three. */
void sub_08199684(s32 value)
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
INCLUDE_ASM("asm/nonmatching", SceneEffectQueue_PostTypeZeroMessage);
#endif
