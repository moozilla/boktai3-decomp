#include "minigames/result_game/result_tutorial.h"

void ResultMenu_EnqueueMarker(struct ResultMenu *menu)
{
    struct ResultMarkerFeedback *feedback = &menu->markerFeedback[menu->nextFeedbackSlot];
    struct AnimSprite *sprite = &menu->widgets[6 + menu->nextFeedbackSlot];
    Anim_LoadKeyframe(sprite, &menu->primaryResource, menu->lastTimingGrade + 0xc2, 0);
    sprite->flags &= ~0x201;
    feedback->phase = 1;
    feedback->elapsed = 0;
    if (++menu->nextFeedbackSlot > 4)
        menu->nextFeedbackSlot = 0;
}
