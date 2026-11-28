#include "effects/healing/healing_effect.h"
#include "engine/engine_globals.h"
#include "ui/hud/damage_number_display.h"
#include "engine/engine_pause_flags.h"

// confidence=low: HealthRestoreEffect_Init's placement for the health burst; also hides the
// owner's sprite and keeps the mode argument (mode 0 creates the sprites).
s32 sub_08235C30(struct HealthBurstEffect *effect, struct Player *owner, u16 mode)
{
    struct ContactPosition position;

    effect->owner = owner;
    effect->mode = mode;
    position = *(struct ContactPosition *)&effect->owner->position;
    position.y += 150;
    DamageNumber_ProjectPosition(&effect->startPosition, &position);
    sub_082359CC(effect);
    HealthBurstEffect_InitParticlePool(effect);
    sub_08235B34(effect);
    if (effect->owner->unk419 == 0)
        effect->owner->animation.flags &= ~1;
    else
        effect->owner->secondaryDrawFlags &= ~1;
    effect->owner->positionResetValue = 0;
    gEngineFlags |= 1;
    Engine_SetPauseFlags(4);
    if (effect->mode == 0)
        HealthBurstEffect_InitSprites(effect);
    effect->state = sub_082358C0;
    effect->timer = 0;
    return 0;
}
