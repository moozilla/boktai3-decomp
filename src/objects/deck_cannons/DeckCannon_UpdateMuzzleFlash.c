#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

void DeckCannon_UpdateMuzzleFlash(struct DeckCannonShot *shot)
{
    if (shot->finished) {
        shot->update = 0;
        if (shot->destroy) {
            Callback_InvokeActor(shot, shot->destroy);
            shot->destroy = 0;
        }
    }
}
