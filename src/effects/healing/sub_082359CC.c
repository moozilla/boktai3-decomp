#include "effects/healing/healing_effect.h"
#include "engine/engine.h"
#include "ui/hud/stage_icon_display.h"

struct IconHeader {
    u32 words[8];
};

void sub_082359CC(struct HealthBurstEffect *effect)
{
    struct IconHeader *source = (struct IconHeader *)ScriptProc_Start(0xCB05, 0x5D04);
    struct IconHeader header = *source;
    u32 kind;

    ResourceHeader_Relocate(&header, source);
    kind = effect->mode == 0 ? 47 : 48;
    effect->targetPosition.x = effect->startPosition.x - 8;
    effect->targetPosition.y = effect->startPosition.y - 40;
    effect->targetPosition.z = effect->startPosition.z;
    AnimSprite_InitLinked(&effect->icon, &header, kind + 104, 50, 1, 0, 60, &effect->startPosition);
    effect->icon.scaleY = 34;
    effect->icon.scaleX = 34;
}
