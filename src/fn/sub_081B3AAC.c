// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
// confidence=low: three motion legs include an approach toward the player.

// confidence=low: inferred from adjacent Vanargand part callbacks and local state transitions.

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: state-transition address reuse, entry-pointer copies, and allocation.
struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x44];
    struct SceneVec origin;
    u8 unknownA4[8];
    s16 heading, distance;
    u8 unknownB0[0xac];
    u32 variant;
    u32 flags;
    u32 state;
    u32 timer;
    u8 unknown16C[2];
    u8 reset, enter, finished;
};
void MotionTrack_Initialize(void *, const void *, const void *, const void *, u32);
void MotionTrack_Update(void *);
extern const s16 gUnk_086149C4[];

struct SceneVec *Player_GetEffectivePosition(u32);
static inline s32 TruncateShift(s32 x, u32 bits)
{
    return x >= 0 ? x >> bits : -((-x) >> bits);
}
static inline void MakeVector(struct SceneVec *out, s32 angle, s32 scale)
{
    out->x = TruncateShift(gUnk_086149C4[(angle + 64) & 255] * scale, 12);
    out->y = 0;
    out->z = TruncateShift(gUnk_086149C4[angle & 255] * scale, 12);
}
static inline u32 MotionEnded(struct VanargandPart *p)
{
    u32 ended = p->motion[0x29] == 2;
    asm("" : "+r"(ended));
    return ended;
}
static inline void AddVector(struct SceneVec *dst, struct SceneVec *a, struct SceneVec *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}
static inline void Difference(struct SceneVec *dst, struct SceneVec *a, struct SceneVec *b)
{
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    u32 next = state;
    p->state = next;
    p->timer = 0;
}
void Vanargand_UpdatePartLunge(void *unused, struct VanargandPart *p)
{
    u8 *enter;
    u32 entering;
    u32 mask = 0x20;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        p->state = 0;
        p->timer = 0;
        p->enter = 1;
    }
    switch (p->state) {
        case 0: {
            struct SceneVec delta;
            entering = p->enter;
            asm("" : "+r"(entering));
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                MakeVector(&delta, 160, 256);
                AddVector(&delta, &p->origin, &delta);
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 4);
            }
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                SetState(p, 1);
                *enter = 1;
            }
            break;
        }

        case 1: {
            struct SceneVec delta;
            entering = p->enter;
            asm("" : "+r"(entering));
            enter = &p->enter;
            if (entering) {
                struct SceneVec *player;
                struct SceneVec *base;
                {
                    u8 k1 = 0;
                    *enter = k1;
                }
                player = Player_GetEffectivePosition(0);
                do {
                    base = *((struct SceneVec **)(((u8 *)p) + 0x90));
                } while (0);
                Difference(&delta, player, base);
                delta.x = TruncateShift(delta.x, 1);
                delta.y = p->origin.y;
                delta.z = TruncateShift(delta.z, 1);
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 3);
            }
            if ((++p->timer) > 16) {
                SetState(p, 2);
                *enter = 1;
            }
            break;
        }

        case 2:
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                SetState(p, 3);
                p->enter = 1;
            }
            break;

        case 3:
            if ((++p->timer) > 30) {
                SetState(p, 4);
                p->enter = 1;
            }
            break;

        case 4: {
            struct SceneVec delta;
            entering = p->enter;
            asm("" : "+r"(entering));
            enter = &p->enter;
            if (entering) {
                *enter = 0;
                MakeVector(&delta, p->heading, p->distance);
                MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 4);
            }
            MotionTrack_Update(p->motion);
            if (MotionEnded(p)) {
                SetState(p, 5);
                *enter = 1;
            }
            break;
        }

        case 5:
            p->finished = 1;
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Vanargand_UpdatePartLunge);
#endif
