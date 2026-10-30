#include "scene_actor_reaction.h"
inline static u8 blocked2048(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

inline static u8 blockedFirst(u8 *c)
{
    u32 mask;
    u32 *p;
    u32 off;
    if (!c)
        goto zero;
    mask = 0x40;
    off = 0xB5;
    off <<= 2;
    p = (u32 *)(c + off);
    if ((*((u16 *)p)) & mask)
        goto one;
    mask = 0x80;
    mask <<= 10;
    off -= 4;
    p = (u32 *)(c + off);
    if ((*p) & mask)
        goto one;
    if ((*((s16 *)(c + 0x9C))) > 0)
        goto zero;
one:
    return 1;
zero:
    return 0;
}

static inline void Delta(struct ContactPosition *out, struct ContactPosition *to,
                         struct ContactPosition *from)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
void Dog_OnAttackContact(u8 *collider, u8 *other, struct SceneActorRegistryResetView *actor)
{
    union SceneActorRegistryPackedPosition position;
    struct ContactPosition delta;
    s32 direction;
    struct SceneActorRegistryResetView *facingActor = actor;
    if (blockedFirst((u8 *)actor))
        return;
    position.position = *(struct ContactPosition *)(collider + 0x24);
    direction = ((facingActor->body.direction + 0x20) & 0xFF) >> 6;
    switch (direction) {
        case 0:
            position.packed.x -= 0x100;
            break;
        case 1:
            position.packed.z -= 0x100;
            break;
        case 2:
            position.packed.x += 0x100;
            break;
        case 3:
            position.packed.z += 0x100;
            break;
    }
    {
        struct ContactPosition *out = &delta;
        struct ContactPosition *from = &position.position;
        struct ContactPosition *to = (struct ContactPosition *)(other + 0x24);
        Delta(out, to, from);
    }
    collider[0x42] = Math_Atan2(delta.x, delta.z);
    if (!blocked2048((u8 *)actor) && (*(u16 *)(other + 0xA) & 0xF)) {
        u32 *flags;
        s32 mask;
        actor->field304 = *(u16 *)(other + 4);
        flags = &actor->flags2D0;
        mask = 0xFEFFFFFF;
        *flags &= mask;
    }
}
