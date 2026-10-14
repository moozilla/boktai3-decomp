#include "orbit_contact_actor.h"
#define OrbitContactActor_UpdateContracting sub_08046900
void OrbitContactActor_UpdateContracting(struct OrbitContactActor *actor)
{
    actor->angle += 6;
    actor->radius -= 7;
    if (actor->radius < 0)
        actor->radius = 0;
    if (Mod(actor->stateFrame, 20) == 8)
        Sound_PlaySongChecked(0x3BC);
    if (actor->stateFrame > 59) {
        actor->state = 0;
        actor->stateFlag = 1;
        actor->stateFrame = 0;
    }
    ++actor->stateFrame;
}
