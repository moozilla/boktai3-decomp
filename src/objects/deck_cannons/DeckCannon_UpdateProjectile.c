#include "global.h"
#include "objects/deck_cannons/deck_cannon_battery.h"

void DeckCannon_UpdateProjectile(struct DeckCannonShot *shot)
{
    switch (shot->direction) {
        case 0:
            shot->sprite.position.z -= 0x18;
            if (shot->startCoordinate - shot->sprite.position.z > 0x1000) {
                shot->update = NULL;
                if (shot->destroy) {
                    Callback_InvokeActor(shot, shot->destroy);
                    shot->destroy = NULL;
                }
            }
            break;
        case 0x10:
            shot->sprite.position.x -= 0x18;
            if (shot->startCoordinate - shot->sprite.position.x > 0x1000) {
                shot->update = NULL;
                if (shot->destroy) {
                    Callback_InvokeActor(shot, shot->destroy);
                    shot->destroy = NULL;
                }
            }
            break;
    }
    Collider_Refresh(&shot->collider);
    Collider_SetPosition(&shot->collider, &shot->sprite.position, 0);
}
