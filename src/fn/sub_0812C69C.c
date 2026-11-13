#include "contact_swarm.h"
#include "engine.h"
#include "ram.h"
#include "player_cue_sequence.h"
struct Fragment {
    u32 flags;
    u8 active, unknown5[3];
    s8 scaleX, scaleY;
    u8 unknownA[2];
    void *graphics;
    u16 frame;
    u8 unknown12[10];
    struct ContactPosition position;
    u8 unknown24[8];
    struct DisplaySequenceState sequence;
    u8 collider[0x54];
    struct ContactPosition center, offset;
    u32 angle, angularSpeed;
    s16 direction;
    u16 unknownAA;
    s16 radius, scaleTimer;
    u32 unknownB0;
    s32 lifetime;
    u16 unknownB8;
    u8 state;
    s8 slot;
    u16 timer;
    u8 entered;
};
struct Pool {
    u8 pad[0x38];
    u32 used;
};
extern const s16 gUnk_086149C4[];
s32 Mod(s32, s32);
s32 Div(s32, s32);
void FragmentEffect_SpawnRandomParticle(struct ContactPosition *);
void FragmentEffect_UpdateCollider(struct Fragment *);
void Sprite_Remove(void *);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline void Step(struct Fragment *sprite)
{
    PlayerCue_StepSequence((struct ContactSwarmSprite *)sprite, &sprite->sequence);
}
static inline void Sum(struct ContactPosition *out, const struct ContactPosition *a,
                       const struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
// confidence=low: circular fragment motion around the saved center, using F422 graphics
void FragmentEffect_UpdateOrbit(struct Pool *unused, struct Fragment *p)
{
    struct ContactPosition *offset;
    struct ContactPosition *position;
    struct ContactPosition *savedOffset;
    u16 *timer;
    u32 angle;
    s32 magnitude;
    if (p->entered) {
        p->timer = 0;
        p->entered = 0;
    }
    if (p->direction > 0) {
        p->angle += p->angularSpeed;
        if (p->angle > 0xFF000)
            p->angle -= 0xFF000;
    } else {
        p->angle -= p->angularSpeed;
    }
    {
        u32 rawAngle = p->angle;
        s32 radius = p->radius;
        offset = &p->offset;
        angle = rawAngle >> 12;
        magnitude = radius << 7;
    }
    {
        s32 product = magnitude * gUnk_086149C4[(angle + 64) & 255];
        s32 value = product;
        savedOffset = offset;
        {
            s32 scaled = Truncate(value);
            offset->x = scaled;
        }
    }
    offset->y = 0;
    {
        s32 product;
        angle &= 255;
        product = magnitude * gUnk_086149C4[angle];
        {
            s32 scaled = Truncate(product);
            offset->z = scaled;
        }
    }
    p->offset.y = 0;
    position = &p->position;
    Sum(position, &p->center, savedOffset);
    {
        u16 *frame = &p->timer;
        s32 remainder = Mod(*frame, 30);
        timer = frame;
        if (!remainder) {
            u32 index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            {
                s32 sample = *(const u8 *)(RANDOM_TABLE_ADDR + index * sizeof(*gRandomTable));
                if (sample <= 127)
                    FragmentEffect_SpawnRandomParticle(position);
            }
        }
    }
    FragmentEffect_UpdateCollider(p);
    Step(p);
    (*timer)++;
    if (p->lifetime > 0)
        p->lifetime--;
    else {
        p->state = 3;
        p->entered = 1;
    }
}
