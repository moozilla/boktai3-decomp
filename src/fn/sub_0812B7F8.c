#include "global.h"
#include "player.h"
#include "pursuing_actor.h"
s32 Math_Atan2(s32, s32);
s32 Mod(s32, s32);
void PursuingActor_TurnTowardPlayer(struct PursuingActor *actor)
{
    struct SceneVec delta;
    struct SceneVec *out;
    struct Player **players;
    struct SceneVec *left, *right;
    s32 angle, difference, magnitude;
    if (actor->turnDelay) {
        actor->turnDelay--;
        return;
    }
    if (!actor->turnTimer)
        return;
    players = gPlayers;
    out = &delta;
    right = &actor->position;
    left = &players[0]->position;
    delta.x = left->x - actor->position.x;
    out->y = left->y - right->y;
    out->z = left->z - right->z;
    angle = Math_Atan2(delta.x, out->z);
    difference = angle - actor->heading;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    magnitude = difference;
    if (difference < 0)
        magnitude = -difference;
    if (magnitude > 8) {
        if (difference > 0)
            actor->heading += actor->turnSpeed;
        else
            actor->heading -= actor->turnSpeed;
    } else {
        actor->turnTimer >>= 1;
        actor->turnSpeed >>= 1;
    }
    if (actor->turnTimer)
        actor->turnTimer--;
    if (Mod(actor->timer, 4) == 0 && actor->turnSpeed)
        actor->turnSpeed--;
}
