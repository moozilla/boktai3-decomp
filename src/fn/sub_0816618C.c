#include "contact_swarm.h"
#include "player_cue_sequence.h"

struct MovingIcon {
    u8 pad000[0x184];
    struct ContactSwarmSprite sprite;
    u8 pad1AC[0x1CC - 0x184 - sizeof(struct ContactSwarmSprite)];
    struct DisplaySequenceState seq;
    const void *resource;
};
void sub_0816618C(struct MovingIcon *p, s16 x, s16 y)
{
    p->sprite.flags &= ~1;
    p->sprite.position.x = x;
    p->sprite.position.y = y;
    p->sprite.position.z = 0;
    Anim_SetSequence(&p->seq, p->resource, 0, 0, 0);
    PlayerCue_StepSequence(&p->sprite, &p->seq);
}
