#include "tile_trigger.h"
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
