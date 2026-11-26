#include "bosses/vanargand/scene_departure_display.h"

void VanargandHead_DestroyTask(void *);

static inline void SetCallback(void (*fn)(void *), void (**slot)(void *))
{
    *slot = fn;
}

void VanargandHead_UpdateLeftDeparture(struct DepartureActor *input, s32 time)
{
    struct DepartureActor *actor = input;
    DepartureActor_EnterDisplay(actor);
    actor->velocity = -0x5000;
    actor->acceleration = 0;
    actor->position.x += actor->velocity;
    if (actor->screen.x < 0) {
        SetCallback(VanargandHead_DestroyTask, &input->callback);
        input->stateTimer = 0;
        input->entered = 1;
        input->phase = 0;
        input->phaseTimer = 0;
    }
    ActorDisplay_UpdateScreenAnchor(input);
    ActorDisplay_UpdateForegroundSprite(input);
    ActorDisplay_UpdateAccentSprite(input);
    VanargandHead_EmitPeriodicParticles(input, time);
}
