#include "global.h"
#include "contact_swarm.h"
#include "scene_vec.h"
#include "player_cue_sequence.h"
// confidence=low: moves a Vanargand part along its heading, then plays sequence 7.
// confidence=low: provisional role inferred from adjacent Vanargand part callbacks.

// Byte-identical with five register pins and five empty constraints.
struct VanargandPart {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x1c];
    struct DisplaySequenceState sequence;
    u8 motion[0x44];
    struct SceneVec origin;
    u8 unknownA4[8];
    s16 heading;
    u8 unknownAE[0xae];
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
static inline s32 TruncateScale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
static inline void RetreatVector(struct SceneVec *out, s32 angle)
{
    s32 scale = 720;
    s32 x = gUnk_086149C4[(angle + 64) & 255] * scale;
    out->x = TruncateScale(x);
    out->y = 0;
    x = gUnk_086149C4[angle & 255] * scale;
    out->z = TruncateScale(x);
}
static inline u32 StepPlayback(struct VanargandPart *owner, struct DisplaySequenceState *state,
                               struct VanargandPart *base, u32 one)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, &base->sequence);
}
static inline void SetState(struct VanargandPart *p, u32 state)
{
    p->state = state;
    p->timer = 0;
}
static inline void SetFlags(struct VanargandPart *p, u32 mask)
{
    p->flags |= mask;
}
void Vanargand_UpdatePartRetreat(u8 *boss, struct VanargandPart *p)
{
    u8 *enter;
    u32 state;
    struct SceneVec delta;
    u32 mask = 0x20;
    p->flags |= mask;
    if (p->reset) {
        p->reset = 0;
        if (p->variant == 0)
            RetreatVector(&delta, p->heading);
        else
            RetreatVector(&delta, p->heading);
        MotionTrack_Initialize(p->motion, &p->origin, &delta, 0, 4);
        {
            u32 zero = 0;
            p->state = zero;
            p->timer = zero;
            p->enter = 1;
        }
    }
    state = p->state;
    switch (state) {
        case 0:
            SetFlags(p, 8);
            MotionTrack_Update(p->motion);
            {
                u32 finished = p->motion[0x29] == 2;
                if (finished) {
                    SetState(p, 1);
                    p->enter = 1;
                }
            }
            break;
        case 1: {
            u8 *localEnter = &p->enter;
            u8 value = *localEnter;
            enter = localEnter;
            if (value) {
                *localEnter = value = 0;
                Anim_SetSequence(&p->sequence, *(void **)(boss + 0x4d0), 7, (u8)p->variant, 0);
            }
        }
            if (StepPlayback(p, &p->sequence, p, state)) {
                SetState(p, 2);
                *enter = 1;
            }
            break;
        case 2:
            p->finished = 1;
            break;
    }
}
