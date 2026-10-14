#include "orbit_contact_actor.h"
void OrbitContactActor_UpdateExpanding(struct OrbitContactActor *actor)
{
    if (actor->stateFlag)
        actor->sprite.flags &= ~1;
    actor->angle += 6;
    actor->radius += 7;
    if (Mod(actor->stateFrame, 20) == 8)
        Sound_PlaySongChecked(0x3BC);
    if (actor->stateFrame > 59) {
        actor->state = 2;
        actor->stateFlag = 1;
        actor->stateFrame = 0;
    }
    ++actor->stateFrame;
}
