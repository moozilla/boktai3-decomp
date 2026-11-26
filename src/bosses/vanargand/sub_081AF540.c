#include "engine/shared/global.h"
#include "engine/shared/scene_actor_init_actor.h"

struct VanargandEffectVec {
    s16 x, y, z;
};
union PackedXY {
    u32 packed;
    s16 xy[3];
};
void Vanargand_ProjectEffectOffset(struct VanargandEffectVec *out, s16 x, s16 y, s32 height)
{
    union PackedXY in;
    s32 screenX, screenY, projected;
    s16 *coordinates;
    u32 low, high;
    low = (u32)x << 16;
    high = (u32)y << 16;
    screenY = (low >> 16) | high;
    in.packed = screenY;
    coordinates = in.xy;
    screenX = in.xy[0];
    screenY = coordinates[1];
    projected = (screenX + screenY) * 32 / 3 + height;
    out->x = MonsterInit_Half(projected);
    out->y = height;
    projected = (screenY - screenX) * 32 / 3 + height;
    out->z = MonsterInit_Half(projected);
}
