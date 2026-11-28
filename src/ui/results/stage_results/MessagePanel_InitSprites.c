#include "global.h"
#include "ui/results/stage_results.h"
#include "engine/engine.h"

s32 ResourceHeader_Relocate(void *, void *);
void AnimSprite_InitLinked(void *, void *, int, int, int, int, int, void *);

void MessagePanel_InitSprites(struct StageResults *results)
{
    s32 i;
    results->spriteSheet = ScriptProc_Start(0xCB05, 0x1289);
    ResourceHeader_Relocate(results->animation, results->spriteSheet);
    for (i = 0; i < 23; i++) {
        AnimSprite_InitLinked(&results->sprites[i], results->animation, results->spriteIds[i], 0x10,
                              1, 0, 0, 0);
        results->selections[i] = 0xFFFF;
        if (i == 5)
            results->sprites[5].visible = 0;
    }
    results->sprites[7].flags |= 1;
}
