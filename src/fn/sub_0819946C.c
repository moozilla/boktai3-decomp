// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"

struct ScreenPosition {
    s16 x, y, z;
};
struct PackedPosition {
    u32 x : 16, y : 16, z : 16, w : 16;
};
struct Message {
    u32 id : 16;
    u32 reserved : 8;
    u32 count : 8;
    s16 *values;
};
extern struct ScreenPosition gCameraOrigin;
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
// fuzzy: 48.7%
// scope shape 256 (differences once low registers are set aside)
// Tentative: post a type-zero scene-effect message for the player's screen side.
void SceneEffectQueue_PostTypeZeroMessage(s32 arg)
{
    register s32 value asm("r8") = arg;
    s16 values[3];
    struct Message message;
    struct PackedPosition in;
    struct ScreenPosition out;
    s32 sum, height, screenY, depth, x, z, zero;
    struct ScreenPosition *input, *output;
    message.id = 0x382f;
    message.count = 3;
    message.values = values;
    in.x = (u16)gGameState->positionX;
    in.y = (u16)gGameState->positionY;
    in.z = (u16)gGameState->positionZ;
    output = &out;
    input = (struct ScreenPosition *)&in;
    x = input->x;
    z = input->z;
    output->x = ScaleProjection(3 * (x - z));
    sum = ScaleProjection(3 * (x + z));
    height = ScaleProjection(3 * input->y);
    screenY = sum - height;
    zero = 0;
    depth = sum + height;
    output->x = output->x - gCameraOrigin.x + 120;
    output->y = screenY - gCameraOrigin.y + 80;
    output->z = depth - gCameraOrigin.z;
    if (out.x > 120)
        values[1] = zero;
    else
        values[1] = 1;
    values[0] = zero;
    values[2] = value;
    MessageQueue_PostMessage(&message);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0819946C);
#endif
