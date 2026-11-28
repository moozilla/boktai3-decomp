#include "global.h"
#include "ui/results/stage_results.h"

s32 AnimSprite_SelectGlyph(void *, void *, u32);
extern const u8 gUnk_086106B0[];

s32 SceneGauge_SelectThresholdFrame(struct StageResults *results, s32 value)
{
    s32 level = -1;
    s32 i;
    for (i = 0; i < 4; i++) {
        if (value >= gUnk_086106B0[i]) {
            level = i;
            break;
        }
    }
    if (level < 0)
        level = 4;
    results->spriteIds[5] = level + 0x22;
    AnimSprite_SelectGlyph(&results->sprites[5], results->animation, results->spriteIds[5]);
    return level;
}
