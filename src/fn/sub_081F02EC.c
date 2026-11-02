#include "slot7_cooldown.h"
#include "game_state.h"

struct Slot7Action {
    u8 pad0[0x3e];
    u16 phase;
    u8 pad40[3];
    u8 state;
    u16 cooldown;
};
void Collision_CalculateDamageResponse(void *, void *);
s32 Div(s32, s32);

void SingletonSlot7_BeginScaledCooldown(void *event, struct Slot7Action *action,
                                        struct Slot7Actor *actor)
{
    s16 level;
    s32 scaledLevel;
    Collision_CalculateDamageResponse(event, action);
    if (action->cooldown) {
        action->phase = 0;
        action->state = 0;
    } else {
        level = gGameState->unk876;
        scaledLevel = level - 3;
        scaledLevel = Div(scaledLevel * 10, 6);
        if (scaledLevel <= 2) {
            actor->duration = actor->baseDuration;
        } else if (scaledLevel <= 5) {
            actor->duration = Slot7_IncreaseDuration(actor->baseDuration, 2);
        } else {
            actor->duration = Slot7_IncreaseDuration(actor->baseDuration, 1);
        }
        action->cooldown = 30;
        actor->flashTimer = 15;
    }
}
