#include "field/stored_resource_station/resource_station.h"
#include "engine/shared/render.h"
s32 Div(s32, s32);
void Anim_LoadKeyframe(void *, void *, u16, u32);
static inline s32 ResourceStation_ProjectScale(s32 value)
{
    if (value >= 0)
        return value >> 5;
    return -((-value) >> 5);
}
static inline void ResourceStation_ProjectPosition(struct ResourceStationPosition *screen,
                                                   const struct ResourceStationPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 sum, vertical, y, depth;
    s32 scaledSum;
    screen->x = ResourceStation_ProjectScale(3 * (x - z));
    scaledSum = ResourceStation_ProjectScale(3 * (x + z));
    sum = scaledSum;
    vertical = ResourceStation_ProjectScale(3 * world->y);
    y = sum - vertical;
    depth = sum + vertical;
    screen->x = (u16)screen->x - gCameraOrigin[0] + 120;
    screen->y = y - gCameraOrigin[1] + 80;
    screen->z = depth - gCameraOrigin[2];
}
void ResourceStation_UpdateDigits(struct ResourceStation *station)
{
    s32 i;
    if (*(u16 *)&station->nearby) {
        s32 digits[4];
        struct ResourceStationPosition screen;
        s32 remainder;
        if (gGameState->unk77C > 9999)
            gGameState->unk77C = 9999;
        digits[0] = Div(gGameState->unk77C, 1000);
        remainder = gGameState->unk77C - digits[0] * 1000;
        digits[1] = Div(remainder, 100);
        remainder -= digits[1] * 100;
        digits[2] = Div(remainder, 10);
        remainder -= digits[2] * 10;
        digits[3] = remainder;
        ResourceStation_ProjectPosition(&screen, &station->sprite.position);
        screen.x -= 16;
        screen.y -= 44;
        for (i = 0; i < 4; i++) {
            station->digits[i].flags &= ~1;
            Anim_LoadKeyframe(&station->digits[i], station->digitGraphics, digits[i] + 30, 1);
            station->digits[i].position = screen;
            screen.x += 8;
        }
    } else {
        for (i = 0; i < 4; i++)
            station->digits[i].flags |= 1;
    }
}
