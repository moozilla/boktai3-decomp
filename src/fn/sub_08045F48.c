#include "contact_sequence_actor.h"
#include "display_sprite_playback.h"
u32 ContactSequenceActor_TakeEntryFlag(struct ContactSequenceActor *);
void ContactSequenceActor_SetFlags(struct ContactSequenceActor *, u32);
void ContactSequenceActor_SetState(struct ContactSequenceActor *, u32);
void Anim_RestartSequence(void *, const void *, u32, u32, u32);
struct ContactSequenceFrame {
    u16 frame, unknown2;
};
extern const struct ContactSequenceFrame gUnk_08605974[];
void ContactSequenceActor_UpdateAppearing(struct ContactSequenceOwner *parent,
                                          struct ContactSequenceActor *owner)
{
    if (ContactSequenceActor_TakeEntryFlag(owner)) {
        owner->sprite.flags &= ~1;
        Anim_RestartSequence(&owner->sequence, parent->sequenceResource,
                             gUnk_08605974[owner->variant & 1].frame, 0, 8);
    }
    if (owner->sequence.index == 3 && owner->sequence.elapsed == 0)
        Sound_PlaySongChecked(0x3B8);
    ContactSequenceActor_SetFlags(owner, 1);
    if (PlayerCue_StepSequenceEnded(&owner->sprite, &owner->sequence))
        ContactSequenceActor_SetState(owner, 2);
}
