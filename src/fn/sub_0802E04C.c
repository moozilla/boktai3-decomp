#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 64.2%
struct Position {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct Actor {
    u8 pad0[0xE];
    u16 flags;
    u8 pad10[0x68 - 0x10];
    struct Position position;
    u8 pad70[0x2EC - 0x70];
    struct Actor *next;
};
struct ActorList {
    u8 pad0[0x1C];
    struct Actor *head;
};
struct Emitter {
    u8 pad0[8];
    u8 kind;
    u8 pad9[3];
    struct Position position, delta;
    void *resource;
    u16 first, second;
};
extern struct ActorList *gUnk_02000480;
extern u32 gUnk_030053F4;
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void sub_0821C8F8(void *, struct Position *, struct Position *, u32, u32, u32);
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
static inline void Polar(const s16 *table, struct Position *v, u32 angle, s32 radius)
{
    v->x = Scale(table[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = Scale(table[angle & 255] * radius);
}
static inline void Add(struct Position *out, struct Position *a, struct Position *b)
{
    out->x = a->x + b->x;
    out->y = a->y + b->y;
    out->z = a->z + b->z;
}
static inline void Subtract(struct Position *out, struct Position *a, struct Position *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
static inline u32 Flag(u16 *field, u32 mask)
{
    return *field & mask;
}
// confidence=low: emits toward flagged nearby actors using an outward radial offset.
#define SceneActorAnimation_EmitTowardNearbyActors sub_0802E04C
void SceneActorAnimation_EmitTowardNearbyActors(struct Emitter *emitter)
{
    struct Actor *actor;
    struct Position offset;
    const s16 *table;
    if (!gUnk_02000480 || (gUnk_030053F4 & 0x200))
        return;
    actor = gUnk_02000480->head;
    if (!actor)
        return;
    table = gUnk_086149C4;

    do {
        if (Flag(&actor->flags, 0x1000)) {
            register struct Position *position asm("r6") = &actor->position;
            struct Position *origin = &emitter->position;
            register struct Position *savedOrigin asm("sb");
            s32 distance;
            s32 dx = origin->x - position->x;
            s32 dz = origin->z - position->z;
            distance = (s32)((u32)dx * dx + (u32)dz * dz);
            savedOrigin = origin;
            if (distance <= 9999) {
                s32 angle = Math_Atan2(dx, dz);
                Polar(table, &offset, angle, 100);
                Add(&offset, position, &offset);
                Subtract(&emitter->delta, &offset, savedOrigin);
                if (emitter->resource)
                    sub_0821C8F8(emitter->resource, savedOrigin, &emitter->delta, emitter->first,
                                 emitter->second, emitter->kind);
            }
        }
        actor = actor->next;
    } while (actor);
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0802E04C);
#endif
