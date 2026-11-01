#include "global.h"
#include "display_animation.h"
#include "scene_vec.h"
#include "player_cue_sequence.h"
struct TileTrigger {
    u32 flags;
    u8 unknown4[0xC];
    u16 spriteFrame;
    u8 unknown12[0x62];
    struct DisplaySequenceState animation;
    struct SceneVec position;
    u8 unknown8C[0x2C];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
};
void sub_08106E98(struct TileTrigger *);
static inline u32 StepPlayback(struct TileTrigger *owner, struct DisplaySequenceState *state,
                               struct TileTrigger *base)
{
    const u16 *command;
    {
        u32 offset = state->index << 1;
        const u16 *table;
        table = base->animation.commands;
        command = (const u16 *)((u32)table + offset);
    }
    return PlayerCue_AdvanceCommand((struct ContactSwarmSprite *)owner, state, command, 1);
}

void TileTrigger_StepPress(struct TileTrigger *p)
{
    struct TileTrigger *self = p;
    p->flags &= ~1;
    if ((p->animation.index == 1) && (p->animation.elapsed != 0)) {
        sub_08106E98(p);
        p->frame++;
    } else {
        StepPlayback(self, &p->animation, p);
    }
    if (p->frame > 30) {
        p->state = 5;
        p->forced = 1;
        p->frame = 0;
    }
}
