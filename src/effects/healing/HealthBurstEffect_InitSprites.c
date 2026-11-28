#include "effects/healing/healing_effect.h"
#include "engine/engine.h"

void HealthBurstEffect_InitSprites(struct HealthBurstEffect *effect)
{
    s32 i;

    AnimData_Load(effect->spriteGraphics, 0x24BA);
    for (i = 0; i < 2; i++) {
        Sprite_Init(&effect->sprites[i], effect->spriteGraphics, 1);
        AnimData_SelectPaletteBank(effect->spriteGraphics, 0x2E5);
        effect->sprites[i].position = *(struct ContactPosition *)&effect->owner->position;
    }
}
