#include "global.h"

/* tentative: copies owner position into a sprite slot and steps its sequence */

#include "display_animation.h"
#include "display_sprite_playback.h"
#include "contact_swarm.h"

struct Pos {
    u32 a, b;
};
struct SeqSprite {
    u32 flags;
    u8 unknown4[0xC];
    u16 frame;
    u8 unknown12[0xA];
    struct Pos position;
    u8 unknown24[8];
};
struct SeqParent {
    u8 unknown0[0x1C];
    struct SeqSprite sprites[8];
    u8 unknown17C[0x1C];
    struct DisplaySequenceState states[8];
};
struct SeqOwner {
    struct Pos position;
    u8 unknown8[2];
    u8 result;
    s8 slot;
};

void ContactSequenceSlot_StepPlayback(struct SeqParent *parent, struct SeqOwner *owner)
{
    struct SeqSprite *sprite;
    struct DisplaySequenceState *state;
    u32 ended;

    sprite = &parent->sprites[owner->slot];
    sprite->position = owner->position;
    sprite->flags &= ~1;
    state = (void *)(owner->slot * sizeof(*state) + (u32)parent);
    state = ((struct SeqParent *)state)->states;
    ended = PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)sprite, state);
    if (ended) {
        u32 st = 2;
        owner->result = st;
    }
}
