#include "effects/healing/healing_effect.h"
#include "engine/engine_globals.h"
#include "engine/video/render.h"
#include "ui/hud/damage_number_display.h"

s32 HealthRestoreEffect_Init(struct HealthRestoreEffect *effect, struct Player *owner)
{
    struct ContactPosition position;
    const struct ContactPosition *world;
    struct ContactPosition *screen;
    s32 scaledSum;
    s32 x, z, sum, vertical, y, depth;
    u32 zero;

    effect->owner = owner;
    position = *(struct ContactPosition *)&owner->position;
    position.y += 150;
    /* DamageNumber_ProjectPosition, written out: the ROM clears the timer value between the
     * screen row and the depth. */
    screen = &effect->startPosition;
    world = &position;
    x = world->x;
    z = world->z;
    screen->x = DamageNumber_ProjectScale(3 * (x - z));
    scaledSum = DamageNumber_ProjectScale(3 * (x + z));
    sum = scaledSum;
    vertical = DamageNumber_ProjectScale(3 * world->y);
    y = sum - vertical;
    zero = 0;
    depth = sum + vertical;
    screen->x = (u16)screen->x - gCameraOrigin[0] + 120;
    screen->y = y - gCameraOrigin[1] + 80;
    screen->z = depth - gCameraOrigin[2];
    HealthRestoreEffect_InitAnimation(effect);
    HealthRestoreEffect_InitParticlePool(effect);
    effect->state = sub_08234DC8;
    effect->timer = zero;
    gEngineFlags |= 1;
    return 0;
}
