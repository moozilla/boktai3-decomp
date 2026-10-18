#include "global.h"
struct ContactActor {
    u8 pad0[0x34];
    u32 flags;
    u8 pad38[0xA];
    u8 direction;
};
struct ContactReaction {
    u8 state;
    u8 active;
    u8 pad2[4];
    u16 timer;
    u8 pad8;
    u8 direction;
};
void Sound_PlaySong(u32);
void PlayerStats_AddContactHundredths(void *, void *, u32);
void sub_0821FF8C(void *);
void ActorReaction_BeginOnContact(struct ContactActor *actor, void *context,
                                  struct ContactReaction *reaction)
{
    reaction->direction = actor->direction;
    sub_0821FF8C(actor);
    {
        u32 one = 1;
        if (actor->flags & one) {
            Sound_PlaySong(0x592);
            reaction->active = one;
            reaction->state = one;
            reaction->timer = 0;
            PlayerStats_AddContactHundredths(actor, context, 0x19);
        }
    }
}
