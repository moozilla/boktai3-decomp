#include "global.h"
struct ActorDrainState {
    void *callback;
    void *resource;
    s16 timer, duration;
    s8 effect;
    u8 unkD, entered, active;
    s32 accumulator;
};
struct DrainActor {
    u8 pad0[0x58];
    s16 health;
    u8 pad5A[0x36];
    s32 increment;
    u8 pad94[0x1C];
    struct ActorDrainState drain;
    u8 padC4[0x530];
    void *owner;
};
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void TimedSpriteEffect_RequestStop(u32);
s8 SpriteSlots_AttachSource(void *);
u8 LinkParticipant_IsLocallySelected(void *);
s32 sub_0811ECCC(struct DrainActor *);
void Modifier_UpdateIdleDrain(void);
s32 ActorDrain_Update(struct DrainActor *actor)
{
    struct ActorDrainState *state = &actor->drain;
    state->active = 1;
    if (state->entered) {
        if (state->resource) state->effect = SpriteSlots_AttachSource(state->resource);
        state->timer = state->duration;
        if (LinkParticipant_IsLocallySelected(actor->owner)) Sound_PlaySong(0x8C);
        state->entered = 0;
    }
    state->timer--;
    if (Mod(state->timer, 4) == 0) {
        s32 amount = sub_0811ECCC(actor);
        s32 health = actor->health - amount;
        if (health < 0) health = 0;
        actor->health = health;
        actor->drain.accumulator += actor->increment;
    }
    if (state->timer < 0 || actor->health <= 0) {
        if (state->effect >= 0) TimedSpriteEffect_RequestStop(state->effect);
        state->effect = -1;
        state->callback = Modifier_UpdateIdleDrain;
        state->entered = 1;
    }
    return 1;
}
