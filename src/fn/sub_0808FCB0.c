#include "scene_actor_chase.h"
struct SlimeContext {
    u8 filler[0x3D4];
    u16 count;
    u8 filler2[0x3DA - 0x3D6];
    u16 threshold;
    u8 filler3[0x40A - 0x3DC];
    u16 flags;
};
struct SlimeFlagBytes {
    u8 filler[0xF];
    u8 flags;
};
static inline u8 Slime_TestFlag(struct SceneActorRegistryResetView *actor)
{
    struct SlimeFlagBytes *p = (struct SlimeFlagBytes *)((u8 *)actor + 0xDC);
    u32 mask = 2;
    if (p->flags & mask)
        return 1;
    return 0;
}
static inline u8 Slime_TestContext(struct SlimeContext *q)
{
    u32 mask = 8;
    if (q->flags & mask)
        return 1;
    return 0;
}
static inline void Slime_SetFlags2D6(struct SceneActorRegistryResetView *actor, u32 bits)
{
    u32 mask = bits;
    u16 *flags = &actor->flags2D6;
    mask |= *flags;
    *flags = mask;
}
u32 Slime_TryEnrage(struct SceneActorRegistryResetView *actor)
{
    struct SlimeContext *q = actor->context;
    s32 dy;
    actor->field2C2 = 0x40;
    if (!Slime_TestFlag(actor))
        return 0;
    dy = actor->body.position.y - ((s16 *)gPlayers[0])[0x32 / 2];
    if (dy < 0)
        dy = -dy;
    if (dy > 0x200)
        return 0;
    if (*(u32 *)((u8 *)actor + 0xF0) > *(u32 *)((u8 *)actor + 0xF8))
        return 0;
    if (actor->health < q->threshold || Slime_TestContext(q)) {
        Slime_SetFlags2D6(actor, 0x80);
        {
            struct SlimeFlagBytes *p = (struct SlimeFlagBytes *)((u8 *)actor + 0xDC);
            u32 mask = 0x10;
            mask |= p->flags;
            p->flags = mask;
        }
        actor->field2C2 = 0x20;
        q->count++;
    } else {
        Slime_SetFlags2D6(actor, 0x100);
    }
    return 1;
}
