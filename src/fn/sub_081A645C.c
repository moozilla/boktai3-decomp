#include "global.h"
#include "input.h"


struct DebugVector {
    s16 x, y, z;
};

static inline s32 SignedHalf(s32 value)
{
    if (value >= 0)
        return value >> 1;
    return -(-value >> 1);
}

static inline void Unproject(struct DebugVector *out, s16 *in, s32 height)
{
    s32 x = in[0];
    s32 y = in[1];
    out->x = SignedHalf(((x + y) << 5) / 3 + height);
    out->y = height;
    out->z = SignedHalf(((y - x) << 5) / 3 + height);
}

void Debug_TestIsometricProjection(void)
{
    s16 in[3];
    struct DebugVector out;
    if (gPadState[1] & 0x100) {
        s32 height = 0;
        s16 *v = in;
        s32 screenY = 1;
        in[0] = height;
        v[1] = screenY;
        v[2] = height;
        Unproject(&out, in, height);
    }
}
