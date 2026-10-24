#include "global.h"
// confidence=low: places forward collider and selects direction-dependent contact bounds.

#include "scene_actor_reset.h"
extern const s16 gUnk_086149C4[];
void Collider_SetPosition(void *, void *, u32);
static inline u8 SuppressCollision(struct SceneActorRegistryResetView *a)
{
    if (a) {
        u32 mask = 0x40;
        if (!(a->flags2D4 & mask)) {
            u32 mask = 0x20000;
            if (!(a->flags2D0 & mask)) {
                if (a->health > 0)
                    return 0;
            }
        }
        return 1;
    }
    return 0;
}
static inline u8 Test(u32 *field, u32 mask)
{
    if (*field & mask)
        return 1;
    return 0;
}
static inline u8 TestContext(u8 *context, u32 mask, u32 **out)
{
    u32 *field = (u32 *)(context + 0x678);
    u32 value = *field & mask;
    *out = field;
    if (value)
        return 1;
    return 0;
}
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
static inline s32 Direction(s32 value)
{
    return value >= 0 ? value >> 6 : -(-value >> 6);
}
void Dog_UpdateCollisionBounds(struct SceneActorRegistryResetView *a)
{
    union SceneActorRegistryPackedPosition position;
    union SceneActorRegistryPackedPosition wide;
    union SceneActorRegistryPackedPosition narrow;
    u8 *context = a->context;
    s32 heading;
    u32 *flags;
    if (SuppressCollision(a))
        return;
    position.position = a->position;
    heading = Direction((a->direction + 32) & 255) << 6;
    {
        if (TestContext(context, 2, &flags)) {
            position.packed.x += Scale(gUnk_086149C4[(heading + 64) & 255] << 9);
            position.packed.z += Scale(gUnk_086149C4[heading & 255] << 9);
        } else {
            position.packed.x += Scale(gUnk_086149C4[(heading + 64) & 255] << 7);
            position.packed.z += Scale(gUnk_086149C4[heading & 255] << 7);
        }
    }
    Collider_SetPosition(context + 0x624, &position, 0);
    context[0x666] = heading;
    position.position = a->position;
    switch (Direction(heading)) {
        case 0:
        case 2:
            if (Test(flags, 2)) {
                wide.packed.x = 240;
                wide.packed.z = 128;
            }
            narrow.packed.x = 120;
            narrow.packed.z = 64;
            break;
        case 1:
        case 3:
            if (Test(flags, 2)) {
                wide.packed.x = 128;
                wide.packed.z = 240;
            }
            narrow.packed.x = 64;
            narrow.packed.z = 120;
            break;
    }
    narrow.packed.y = 100;
    {
        struct ContactCollider *c = &a->colliderB;
        c->bounds = *(struct ContactPosition *)&narrow;
        Collider_SetPosition(c, &position, 0);
    }
    {
        struct ContactCollider *c = &a->colliderA;
        c->bounds = *(struct ContactPosition *)&narrow;
        Collider_SetPosition(c, &position, 0);
    }
    if (Test(flags, 2))
        ((struct ContactCollider *)(context + 0x624))->bounds = *(struct ContactPosition *)&wide;
    else
        ((struct ContactCollider *)(context + 0x624))->bounds = *(struct ContactPosition *)&narrow;
}
