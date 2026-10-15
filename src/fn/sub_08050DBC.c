#include "orbit_background.h"
// MATCHING_CONCESSION: register-pinning
#define OrbitBackground_Tick sub_08050DBC
static inline s32 scale(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -(-value >> shift);
}
s32 OrbitBackground_Tick(struct OrbitBackground *background)
{
    struct ContactPosition screen;
    sub_08050910(background);
    if (background->enabled) {
        struct ContactPosition *projected;
        struct ContactPosition *position;
        sub_080509E8(background);
        projected = &screen;
        position = &background->position;
        {
            s32 x = background->position.x;
            s32 z;
            z = position->z;
            projected->x = scale((x - z) * 3, 5);
            {
                s32 diagonal = x + z;
                s32 average;
                s32 sum;
                s32 vertical;
                average = scale(diagonal * 3, 5);
                sum = average;
                vertical = scale(position->y * 3, 5);
                projected->y = sum - vertical;
                {
                    s32 depth = sum + vertical;
                    projected->z = depth;
                }
            }
        }
        {
            register s32 sourceX asm("r1") = gUnk_03005418[0];
            register s32 screenX asm("r0") = screen.x;
            register s32 x asm("r4");
            s32 centerX;
            s32 sourceY;
            register s32 screenY asm("r1");
            register s32 y asm("r0");
            s32 centerY;
            x = sourceX - screenX;
            centerX = x + 128;
            sourceY = gUnk_03005418[1];
            screenY = screen.y;
            y = sourceY - screenY;
            centerY = y + 128;
            {
                s32 horizontal = scale(x, 3);
                s32 vertical = scale(centerY - 128, 3) & ~1;
                s32 destination;
                centerX -= 120;
                destination = centerY;
                destination -= 80;
                BgGfx_LoadTilemapSimple(1, horizontal, vertical, centerX, destination);
            }
        }
    }
    return 0;
}
