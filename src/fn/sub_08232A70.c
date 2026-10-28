#include "global.h"
#include "render.h"
// confidence=low: projects the source and positions a timed icon around the screen center.
#define ScreenOrbitEffect_UpdateTrackingSlot sub_08232A70
struct OrbitVec {
    s16 x, y, z, pad;
};
struct OrbitSlot {
    u8 state, angle, enter, unknown3;
    u32 age, flags;
    u8 unknownC[3], priority;
    u8 unknown10[0x10];
    struct OrbitVec position;
    u8 unknown28[8];
    struct OrbitVec world;
};
struct OrbitOwner {
    u8 unknown0[0x18];
    struct OrbitVec center;
};
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
static inline s32 Scale32(s32 n)
{
    return n >= 0 ? n >> 5 : -(-n >> 5);
}
static inline s32 Scale4096(s32 n)
{
    return n >= 0 ? n >> 12 : -(-n >> 12);
}
static inline void Place(struct OrbitSlot *p)
{
    p->flags &= ~1;
    p->position.x = Scale4096(gUnk_086149C4[(p->angle + 64) & 255] * 50) + 120;
    p->position.y = Scale4096(gUnk_086149C4[p->angle] * 50) + 80;
    p->position.z = 0;
    p->priority = 160 - p->angle;
}
static inline void Project(struct OrbitVec *out, struct OrbitVec *world)
{
    s32 sum;
    s32 height;
    s32 low;
    s32 x = world->x;
    s32 z = world->z;
    out->x = Scale32((x - z) * 3);
    sum = Scale32((x + z) * 3);
    height = Scale32(world->y * 3);
    low = sum - height;
    sum += height;
    out->x = out->x - (*(struct OrbitVec *)gCameraOrigin).x + 120;
    out->y = low - (*(struct OrbitVec *)gCameraOrigin).y + 80;
    /* The translated depth is stored as a halfword. */
    sum -= (u16)(*(struct OrbitVec *)gCameraOrigin).z;
    out->z = sum;
}

static inline u32 DistanceSquared(struct OrbitVec *p, struct OrbitOwner *owner_)
{
    struct OrbitVec *center = &owner_->center;
    s32 dx = p->x - center->x;
    s32 dz = p->z - center->z;
    return (dx * dx) + (dz * dz);
}

void ScreenOrbitEffect_UpdateTrackingSlot(struct OrbitOwner *owner, struct OrbitSlot *input)
{
    struct OrbitVec projected;
    struct OrbitVec *center;
    struct OrbitVec *screen;
    if (input->enter)
        input->enter = 0;
    Project(&projected, &input->world);
    center = &owner->center;
    screen = &projected;
    input->angle = Math_Atan2(projected.x - owner->center.x, screen->z - center->z);
    {
        if ((DistanceSquared(&projected, owner) <= 4095) || ((input->age & 31) > 16))
            input->flags |= 1;
        else
            Place(input);
    }
    if (input->age > 179) {
        input->state = 0;
        input->age = 0;
        input->enter = 1;
    }
    input->age++;
}
