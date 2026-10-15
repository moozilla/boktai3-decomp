#include "scene_input_grid.h"
s32 SceneInput_TickPairTransition(struct SceneInputPair *pair)
{
    pair->frame++;
    if (pair->frame >= pair->duration) {
        pair->widgets[0].flags |= 1;
        pair->widgets[1].flags |= 1;
        return 1;
    } else {
        s32 remaining = pair->duration - pair->frame;
        s32 divisor = remaining + 1;
        struct SceneInputPosition *target = pair->targets;
        struct SceneInputMovingWidget *widget = (void *)&pair->widgets[0].flags;
        s32 i = 1;
        do {
            if (!(widget->flags & 1)) {
                widget->position.x = Div(widget->position.x * remaining + target->x, divisor);
                widget->position.y = Div(widget->position.y * remaining + target->y, divisor);
            }
            target++;
            widget++;
            i--;
        } while (i >= 0);
        return 0;
    }
}
