#include "global.h"
#include "player_cue_sequence.h"
#include "scene_vec.h"
struct TileTrigger {
    struct ContactSwarmSprite sprite;
    u8 unknown2C[0x74 - 0x2C];
    struct DisplaySequenceState animation;
    struct SceneVec position;
    u8 unknown8C[0x2C];
    u16 sequence, delay, unknownBC;
    s16 state;
    u16 triggerFlags;
    u8 forced, frame;
};
void sub_08106E98(struct TileTrigger *);
void TileTrigger_StepPress(struct TileTrigger *p)
{
    struct ContactSwarmSprite *sprite = &p->sprite;
    sprite->flags &= ~1;
    if ((p->animation.index == 1) && (p->animation.elapsed != 0)) {
        sub_08106E98(p);
        p->frame++;
    } else {
        PlayerCue_StepSequence(sprite, &p->animation);
    }
    if (p->frame > 30) {
        p->state = 5;
        p->forced = 1;
        p->frame = 0;
    }
}
