#include "global.h"
#include "engine.h"
#include "ram.h"
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
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
void Terrain_MoveFootprint(void *, struct Position *, struct Position *, u16, u16, u8);
static inline s32 Scale(s32 value)
{
    return value >= 0 ? value >> 12 : -(-value >> 12);
}
static inline void Polar(const s16 *table, struct Position *v, u32 angle, const s32 radius)
{
    u32 heading = angle;
    s32 length = radius;
    v->x = Scale(table[(heading + 64) & 255] * length);
    v->y = 0;
    v->z = Scale(table[heading & 255] * length);
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
static inline void Add(struct Position *out, struct Position *a, struct Position *b)
{
    out->x = b->x + a->x;
    out->y = (u16)b->y + (u16)a->y;
    out->z = b->z + a->z;
}

void SceneActorAnimation_EmitTowardNearbyActors(struct Emitter *emitter)
{
    struct Actor *actor;
    struct Position offset;
    const s16 *table;
    if ((!((struct ActorList *)gSceneActorListManager)) || (gEngineFlags & 0x200))
        return;
    actor = ((struct ActorList *)gSceneActorListManager)->head;
    if (!actor)
        return;
    table = gUnk_086149C4;
    do {
        if (Flag(&actor->flags, 0x1000)) {
            struct Position *position = &actor->position;
            struct Position *origin = &emitter->position;
            s32 distance;
            s32 dx = origin->x - position->x;
            s32 dz = origin->z - position->z;
            distance = (s32)((((u32)dx) * dx) + (((u32)dz) * dz));
            if (distance <= 9999) {
                Polar(table, &offset, Math_Atan2(dx, dz), 100);
                Add(&offset, position, &offset);
                Subtract(&emitter->delta, &offset, &emitter->position);
                if (emitter->resource)
                    Terrain_MoveFootprint(emitter->resource, &emitter->position, &emitter->delta,
                                 emitter->first, emitter->second, emitter->kind);
            }
        }
        actor = actor->next;
    } while (actor);
}
