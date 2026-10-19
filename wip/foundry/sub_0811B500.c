#define Collision_DispatchOverlaps sub_0811B500
#define Bounds_TestOverlap sub_0811B498
#include "global.h"
struct CollisionPosition { s32 x, y, z, pad; };
struct CollisionExtent { s16 x, y, z; };
struct CollisionBody {
    u32 flags;
    void *owner;
    u8 pad8[6];
    u8 hit;
    u8 padF;
    struct CollisionExtent position;
    struct CollisionExtent extent;
    struct CollisionExtent offset;
    u8 pad28[10];
    u16 hitTimer;
    u8 pad34[4];
    void *argument;
    void *callback;
};
struct CollisionBodyLink { struct CollisionBody *body; void *previous; struct CollisionBodyLink *next; };
s32 sub_0811B498(struct CollisionPosition *, struct CollisionExtent *, struct CollisionPosition *, struct CollisionExtent *);
s32 sub_08249240(void *, void *, void *);
void sub_0811B500(struct CollisionBody *self, struct CollisionBodyLink *link)
{
    struct CollisionPosition a, b;
    struct CollisionBody *other;
    s32 overlap;
    a.x = self->position.x + self->offset.x;
    a.y = self->position.y + self->offset.y;
    a.z = self->position.z + self->offset.z;
    while (link) {
        other = link->body;
        if ((other->flags & 3) || other == self || other->owner == self->owner || other->hit == 1) {
            link = ((struct CollisionBodyLink **)link)[2];
        } else {
        b.x = other->position.x + other->offset.x;
        b.y = other->position.y + other->offset.y;
        b.z = other->position.z + other->offset.z;
        overlap = sub_0811B498(&a, &self->extent, &b, &other->extent);
        if (overlap == 1 && (other->flags & self->flags)) {
            u16 callbackMask = 0x40;
            if (!(other->flags & callbackMask))
                sub_08249240(self->argument, other, self->callback);
            if (!(self->flags & callbackMask)) {
                other->hit = overlap;
                other->hitTimer = 10;
                sub_08249240(other->argument, self, other->callback);
            }
        }
        link = link->next;
        }
    }
}
