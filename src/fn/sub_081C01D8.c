#include "global.h"
#include "projected_display.h"
#include "render.h"
struct Vec {
    s16 x, y, z;
};
struct Marker {
    u8 pad0[0x20];
    u32 flags;
    u8 pad24[0x14];
    struct Vec screen;
    u8 pad40[0x58];
    struct Vec projected;
    u8 pada0[2];
    s16 visible;
    s32 request;
    u8 *actor;
};
struct Vec *TetherActor_GetPosition(void);
s32 DirectionMarker_Update(void *);
s32 sub_081C0048(void *);
static inline s32 actorBlocked(u8 *actor)
{
    u32 mask = 2;
    return *(u32 *)(actor + 0x9c) & mask;
}
static inline void projectPosition(struct Vec *projected, struct Vec *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 scaled;
    s32 sum, vertical, y, depth;
    projected->x = projectScale(3 * (x - z));
    scaled = projectScale(3 * (x + z));
    sum = scaled;
    vertical = projectScale(3 * world->y);
    y = sum - vertical;
    depth = sum + vertical;
    projected->x = (u16)projected->x - gCameraOrigin[0] + 120;
    projected->y = y - gCameraOrigin[1] + 80;
    projected->z = depth - gCameraOrigin[2];
}
void TetherActor_PositionRequestIndicator(struct Marker *p)
{
    struct Vec *screen = &p->screen;
    struct Vec *world = TetherActor_GetPosition();
    struct Vec *projected = &p->projected;
    projectPosition(projected, *(&world));
    switch (DirectionMarker_Update(p)) {
        case 0:
            screen->x = (u16)p->projected.x - 32;
            screen->y = (u16)p->projected.y + 32;
            break;
        case 1:
            screen->x = (u16)p->projected.x - 32;
            screen->y = (u16)p->projected.y - 32;
            break;
        case 2:
            screen->x = (u16)p->projected.x + 32;
            screen->y = (u16)p->projected.y - 32;
            break;
        case 3:
            screen->x = (u16)p->projected.x + 32;
            screen->y = (u16)p->projected.y + 32;
            break;
    }

    if (screen->x < 16) {
        screen->x = 16;
    } else if (screen->x > 208) {
        screen->x = 208;
    }
    if (screen->y < 16) {
        screen->y = 16;
    } else if (screen->y > 128) {
        screen->y = 128;
    }
    if (!sub_081C0048(p) || actorBlocked(p->actor) || p->request <= 0) {
        p->visible = 0;
        p->flags |= 1;
    } else {
        if (!p->visible) {
            p->visible = 1;
            Sound_PlaySong(0x4d9);
        }
        p->flags &= ~1;
    }
}
