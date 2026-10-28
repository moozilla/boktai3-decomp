#include "global.h"
#include "render.h"

// confidence=low: projects the owner's position to the screen, then runs each of four parts'
// state handlers from a table and places the active parts relative to that screen point.
// sub_08231EF8 and sub_082320C0 are the same update with other handler tables.
struct Vec50 {
    s32 x : 16, y : 16, z : 16;
};
struct ScreenPoint {
    u16 x, y, z, pad;
};
struct EffectPart;
struct EffectGroup;
typedef void (*EffectPartHandler)(void *, struct EffectGroup *, struct EffectPart *);
struct EffectPart {
    u8 state;
    u8 pad1;
    u16 timer;
    u8 pad4[4];
    u16 x, y, z;
    u8 padE[0x28 - 0xe];
    u16 screenX, screenY, screenZ;
    u8 pad2E[0x48 - 0x2e];
};
struct EffectGroup {
    u8 pad0[8];
    struct Vec50 *position;
    struct EffectPart parts[4];
};
extern EffectPartHandler const gUnk_08E879B4[];
static inline s32 scale50(s32 v)
{
    if (v >= 0)
        v = v >> 5;
    else
        v = -((-v) >> 5);
    return v;
}
void sub_08231FDC(void *context, struct EffectGroup *group)
{
    struct ScreenPoint screen;
    u16 *dest;
    s16 *coords;
    s32 x, z, sum, vertical, y, depth;
    struct EffectPart *part;
    struct EffectPart *state;
    s32 i;
    EffectPartHandler const *table;
    struct ScreenPoint *point;
    dest = (u16 *)&screen;
    coords = (s16 *)group->position;
    x = coords[0];
    z = coords[2];
    dest[0] = scale50((x - z) * 3);
    sum = scale50((x + z) * 3);
    vertical = scale50(coords[1] * 3);
    y = sum - vertical;
    depth = sum + vertical;
    dest[0] = dest[0] - gCameraOrigin[0] + 120;
    dest[1] = y - gCameraOrigin[1] + 80;
    dest[2] = depth - gCameraOrigin[2];
    table = gUnk_08E879B4;
    point = &screen;
    part = group->parts;
    state = part;
    i = 3;
    do {
        table[state->state](context, group, part);
        if (state->state) {
            state->screenX = point->x + state->x;
            state->screenY = state->y + point->y;
            state->screenZ = point->z + state->z;
            state->timer++;
        }
        state++;
        part++;
        i--;
    } while (i >= 0);
}
