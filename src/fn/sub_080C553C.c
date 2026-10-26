#include "global.h"
#include "scene_actor_reset.h"
#include "engine.h"
extern u8 *gUnk_02000580[];
s32 Math_Atan2(s32, s32);
static inline u32 Flag(struct SceneActorRegistryResetView *a, u32 m, u32 off)
{
    return off == 0x2D4 ? *(u16 *)((u8 *)a + off) & m : *(u32 *)((u8 *)a + off) & m;
}
static inline u8 Unavailable(struct SceneActorRegistryResetView *a)
{
    if (a && (Flag(a, 0x40, 0x2D4) || Flag(a, 0x20000, 0x2D0) || a->health <= 0))
        return 1;
    return 0;
}
static inline void RecordContact(struct SceneActorRegistryResetView *a,
                                 struct ContactCollider *other)
{
    if (!Unavailable(a) && (other->contactFlags & 15)) {
        *(u32 *)((u8 *)a + 0x304) = *(u16 *)((u8 *)other + 4);
        {
            u32 *p = &a->flags2D0;
            s32 mask = 0xFEFFFFFF;
            *p &= mask;
        }
    }
}
static inline u8 IsFrontal(u8 direction, struct ContactCollider *other)
{
    s32 a, b, delta;
    {
        u8 *owner = other->owner;
        u32 mask = 8;
        if (!(*(u32 *)(owner + 0x20) & mask))
            return 1;
    }
    a = (direction + 0x20) & 255;
    a = a >= 0 ? a >> 6 : -(-a >> 6);
    b = (other->direction + 0x20) & 255;
    b = b >= 0 ? b >> 6 : -(-b >> 6);
    delta = a - b;
    if (delta < 0)
        delta = -delta;
    if (delta == 2)
        return 1;
    return 0;
}
static inline u32 PlayerFlag(u8 *p, u32 mask)
{
    return *(u32 *)(p + 0x20) & mask;
}
static inline u8 Coop(void)
{
    if (gEngineFlags & 0x4800)
        return 1;
    return 0;
}
static inline void AddImpact(u8 *state)
{
    u16 *p = (void *)(state + 0x688);
    *p += 200;
    if (*p > 400)
        *p = 400;
}
static inline void Delta(struct ContactPosition *out, const struct ContactPosition *b,
                         const struct ContactPosition *a)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
void Golem_OnAttackCollision(struct ContactCollider *self, struct ContactCollider *other,
                             struct SceneActorRegistryResetView *a)
{
    struct SceneActorRegistryResetView *base = a;
    u8 *state = a->context;
    union {
        struct ContactPackedVector packed;
        struct ContactPosition pos;
    } origin;
    struct ContactPosition delta;
    if (Unavailable(a))
        return;
    origin.pos = *(struct ContactPosition *)&self->x;
    switch ((s32)((base->direction + 0x20) & 255) >> 6) {
        case 0:
            origin.packed.x -= 256;
            break;
        case 1:
            origin.packed.z -= 256;
            break;
        case 2:
            origin.packed.x += 256;
            break;
        case 3:
            origin.packed.z += 256;
            break;
    }
    Delta(&delta, &origin.pos, (void *)&other->x);
    self->direction = Math_Atan2(delta.x, delta.z);
    if (other->contactFlags & 15) {
        if (Flag(base, 0x100, 0x2D4)) {
            if (base->field2B5 == 0x29 || IsFrontal(base->direction, other)) {
                RecordContact(a, other);
                AddImpact(state);
                {
                    u32 mask = 1;
                    other->responseFlags = mask | other->responseFlags;
                }
                {
                    u16 id = *(u16 *)((u8 *)other + 4);
                    *(u16 *)(state + 0x634) = id;
                }
            } else {
                u8 *player;
                if (Coop()) {
                    s32 index;
                    if (other->contactFlags & 1)
                        index = 0;
                    else if (other->contactFlags & 2)
                        index = 1;
                    else if (other->contactFlags & 4)
                        index = 2;
                    else
                        index = 3;
                    player = gUnk_02000580[index];
                } else
                    player = gUnk_02000580[0];
                if (!PlayerFlag(player, 0x20000)) {
                    u16 *p = (void *)(state + 0x688);
                    u32 value = 400;
                    *p = value;
                }
            }
        } else {
            RecordContact(a, other);
            AddImpact(state);
        }
    } else
        RecordContact(a, other);
}
