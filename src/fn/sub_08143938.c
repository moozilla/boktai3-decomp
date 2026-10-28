#include "global.h"
#include "player.h"
#include "tether_actor.h"
struct MovementSpeedContext {
    u8 unknown0[0x1E];
    u16 reduction;
};
s32 Div(s32, s32);
static inline u8 IsFixedSpeed(struct Player *p)
{
    u16 active = p->active;
    u8 fixed = 0;
    if (active && p->sceneMode == 2)
        fixed = 1;
    return fixed;
}
s32 Player_CalculateMovementSpeed(struct Player *p)
{
    s32 speed;
    if (p && IsFixedSpeed(p))
        speed = 16;
    else {
        speed = p->derivedStats[2];
        speed += 100;
        speed -= p->movementLoadPercent;
        speed -= ((struct MovementSpeedContext *)gTetherActor)->reduction;
        speed = Div(speed * 12, 100);
        if (speed > 16)
            speed = 16;
        else if (speed < 6)
            speed = 6;
    }
    if (p->movementModifierKind == 2 && p->movementModifierActive)
        speed -= 3;
    if (p->movementSpeedReduction)
        speed -= p->movementSpeedReduction;
    if (speed < 2)
        speed = 2;
    return speed;
}
