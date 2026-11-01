#include "global.h"
#include "display_animation.h"
#include "player_cue_sequence.h"

struct DirectionalEffect {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[10];
    u16 x;
    u8 unknown1E[2];
    u16 y;
    u8 unknown22[0x26];
    struct DisplaySequenceState sequence;
    u8 unknown58[4];
    u16 dx;
    u8 unknown5E[2];
    u16 dy;
    u8 unknown62[2];
    u8 active;
};

static inline u32 StepPlayback(struct DirectionalEffect *owner, struct DisplaySequenceState *state,
                               struct DirectionalEffect *base, u32 one)
{
    return PlayerCue_StepSequenceEnded(owner, &base->sequence);
}
void DirectionalEffect_Update(struct DirectionalEffect *p)
{
    if (p->active) {
        if (StepPlayback(p, &p->sequence, p, 1)) {
            p->active = 0;
            {
                struct DirectionalEffect *q = p;
                q->flags |= 1;
            }
        } else {
            p->x += p->dx;
            p->y += p->dy;
        }
    }
}
