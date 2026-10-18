#define gEngineModeFlags             gUnk_030053F4
#define Player_GetSceneStatThreshold Player_GetSelectedSlotActionScale
#include "global.h"
#include "player.h"
extern u32 gUnk_030053F4;
s32 Player_GetSelectedSlotActionScale(void);
s32 ActorFlags_SelectResponse(struct Player *);
s32 Player_UpdateStatThreshold(struct Player *p)
{
    s32 satisfied = 0;
    s32 current = p->currentStatB;
    s32 threshold;
    if (gUnk_030053F4 & 0x1000) {
        threshold = Player_GetSelectedSlotActionScale();
        if ((u8)(p->thresholdMode - 6) <= 5 && current >= threshold)
            satisfied = 1;
        if (satisfied) {
            p->statModifierContext.thresholdSatisfied = 1;
            return threshold;
        }
    } else {
        threshold = ActorFlags_SelectResponse(p);
    }
    p->statModifierContext.thresholdSatisfied = 0;
    return threshold;
}
