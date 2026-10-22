#include "global.h"

struct BoundsPosition {
    s32 x, y, z, w;
};
struct BoundsExtent {
    s16 x, y, z;
};
struct CollisionBody {
    u32 flags;
    void *owner;
    u8 pad8[6];
    u8 hit;
    u8 padF;
    s16 x, y, z;
    u8 pad16[2];
    struct BoundsExtent extent;
    s16 offX, offY, offZ;
    u8 pad26[0x32 - 0x26];
    s16 hitTimer;
    u8 pad34[4];
    void *context;
    void (*onHit)(void *, struct CollisionBody *);
};
struct CollisionBodyLink {
    struct CollisionBody *body;
    void *previous;
    struct CollisionBodyLink *next;
};
s32 Bounds_TestOverlap(struct BoundsPosition *a, struct BoundsExtent *ae, struct BoundsPosition *b,
                       struct BoundsExtent *be);

static inline u32 tst(u32 *a, u32 m)
{
    return *a & m;
}

void Collision_DispatchOverlaps(struct CollisionBody *body, struct CollisionBodyLink *link)
{
    struct BoundsPosition a, b;
    struct CollisionBody *other;
    s32 r;

    a.x = body->x + body->offX;
    a.y = body->y + body->offY;
    a.z = body->z + body->offZ;
    while (link) {
        other = link->body;
        if (tst(&other->flags, 3) || other == body || other->owner == body->owner ||
            other->hit == 1) {
            link = link->next;
            continue;
        }
        b.x = other->x + other->offX;
        b.y = other->y + other->offY;
        b.z = other->z + other->offZ;
        r = Bounds_TestOverlap(&a, &body->extent, &b, &other->extent);
        if (r == 1 && (other->flags & body->flags)) {
            if (!(other->flags & 0x40))
                body->onHit(body->context, other);
            if (!(body->flags & 0x40)) {
                other->hit = r;
                other->hitTimer = 10;
                other->onHit(other->context, body);
            }
        }
        link = link->next;
    }
}
