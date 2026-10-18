#include "global.h"
#define MotionActor_UpdateMovementAudio sub_08116870
#define MotionActor_ApplyPendingSong Character_UpdateLoopingSound
struct MotionActorState {
    u8 unknown0[0xAC];
    u8 blocked;
    u8 unknownAD[3];
    void *callback;
    u8 unknownB4[0x470];
    u16 requestedSong;
    u8 unknown526[0x86];
    s32 movement;
    u8 unknown5B0[4];
    s32 speed;
};
struct MotionActor {
    u8 unknown0[0x18];
    struct MotionActorState state;
    u8 unknown5D0[0x2F4];
    u16 *settings;
};
s32 sub_0811CD70(struct MotionActorState *, u32);
s32 sub_0824923C(void *, void *);
s32 sub_0811E8F0(struct MotionActorState *, s32);
u8 sub_08116C74(struct MotionActor *);
void Character_UpdateLoopingSound(struct MotionActorState *);
s32 Div(s32, s32);
void sub_08116870(struct MotionActor *actor)
{
    struct MotionActorState *state = &actor->state;
    s32 status = sub_0811CD70(state, *actor->settings);
    s32 changed = 0;
    if (sub_0824923C(state, state->callback)) {
        if (state->requestedSong == 0x89)
            state->requestedSong = 0x8B;
        else
            state->requestedSong = 0x8A;
        changed = 1;
    }
    if (state->blocked == 0 && status == 0) {
        state->movement -= sub_0811E8F0(state, 0x20);
        if (!changed)
            state->requestedSong = 0x87;
    }
    state->speed = Div(state->speed * 9, 10);
    if (sub_08116C74(actor))
        Character_UpdateLoopingSound(state);
}
