#include "effects/healing/healing_effect.h"
#include "engine/engine.h"
#include "ui/hud/stage_icon_display.h"

struct IconHeader {
    u32 words[8];
};

u16 Selection_GetDisplayFrame(u32);

void HealthRestoreEffect_InitAnimation(struct HealthRestoreEffect *effect)
{
    struct IconHeader *source = (struct IconHeader *)ScriptProc_Start(0xCB05, 0x5D04);
    struct IconHeader header = *source;

    ResourceHeader_Relocate(&header, source);
    effect->targetPosition.x = 8;
    effect->targetPosition.y = 32;
    effect->targetPosition.z = 0;
    AnimSprite_InitLinked(&effect->icon, &header, Selection_GetDisplayFrame(0x26), 0x33, 0, 0, 0x3C,
                          &effect->targetPosition);
}
