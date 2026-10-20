#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Ratatosk {
    u8 pad0[0xaa];
    u8 animation;
    u8 padAB[4];
    u8 finished, active;
    u8 padB1[0x13];
    u32 timer;
};
struct Vec *Player_GetEffectivePosition(u32);
struct Vec *Ratatosk_FindPointFarthestFromPosition(void *, void *);
struct Vec *Ratatosk_GetRandomMovementPoint(void *);
static inline u8 consume(struct Ratatosk *p)
{
    if (p->active) {
        p->active = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}
#define W(o)                           (*(s32 *)(p + (o)))
#define H(o)                           (*(s16 *)(p + (o)))
#define V(o)                           (*(struct Vec *)(p + (o)))
#define Ratatosk_UpdateRepeatedRetreat sub_081F8EA4
// confidence=low: repeats a retreat animation before returning to a random movement point.
void Ratatosk_UpdateRepeatedRetreat(u8 *p)
{
    struct Ratatosk *actor = (struct Ratatosk *)p;
    if (consume(actor)) {
        u32 animation = 10;
        actor->animation = animation;
        W(0x180) = 0;
        H(0xede) = 0;
    }
    switch ((u32)W(0x180)) {
        case 0:
            if (actor->finished) {
                if (++H(0xede) > 1) {
                    {
                        struct Vec candidate, best;
                        s32 maximum = 0;
                        s16 i;
                        for (i = -1; i <= 1; i += 2) {
                            s32 dx, dz, distance;
                            struct Vec *v = &candidate;
                            struct Vec *self = (struct Vec *)(p + 0x36c);
                            *v = *Player_GetEffectivePosition(0);
                            v->x = v->x + i * 512;
                            dx = (&candidate)->x - self->x;
                            dz = (&candidate)->z - self->z;
                            distance = dx * dx + dz * dz;
                            if (distance >= maximum) {
                                maximum = distance;
                                best = candidate;
                            }
                        }
                        for (i = -1; i <= 1; i += 2) {
                            s32 dx, dz, distance;
                            struct Vec *v = &candidate;
                            struct Vec *self = (struct Vec *)(p + 0x36c);
                            *v = *Player_GetEffectivePosition(0);
                            v->z = v->z + i * 512;
                            dx = (&candidate)->x - self->x;
                            dz = (&candidate)->z - self->z;
                            distance = dx * dx + dz * dz;
                            if (distance >= maximum) {
                                maximum = distance;
                                best = candidate;
                            }
                        }
                        V(0xe5c) = best;
                    }
                    H(0xe6c) = 16;
                    actor->animation = 4;
                    W(0x180) = 1;
                } else {
                    u32 animation = 10;
                    actor->animation = animation;
                }
            }
            break;
        case 1:
            if (actor->finished) {
                W(0xf04) = 20;
                {
                    u32 animation = 1;
                    actor->animation = animation;
                }
                W(0x180) = 2;
            }
            break;
        case 2:
            if (actor->finished) {
                u32 animation = 7;
                actor->animation = animation;
                W(0x180) = 3;
            }
            break;
        case 3:
            if (actor->finished) {
                W(0xf04) = 120;
                {
                    u32 animation = 1;
                    actor->animation = animation;
                }
                W(0x180) = 4;
            }
            break;
        case 4:
            if (actor->finished) {
                struct Vec *point = Ratatosk_GetRandomMovementPoint(p);
                if (point) {
                    V(0xe5c) = *point;
                    H(0xe6c) = 64;
                }
                {
                    u32 animation = 4;
                    actor->animation = animation;
                }
                W(0x180) = 5;
            }
            break;
        case 5:
            if (actor->finished)
                W(0x184) = 1;
            break;
    }
    actor->timer++;
}
