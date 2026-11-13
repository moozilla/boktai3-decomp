#include "scene_input_grid.h"
void SceneInput_BeginPairTransition(struct SceneInputPair *pair, const u32 *enabledInput,
                                    const struct SceneInputPosition *startsInput,
                                    const struct SceneInputPosition *targetsInput,
                                    const u32 *frames, const u32 *unused, u32 duration)
{
    s32 i = 0;
    const struct SceneInputPosition *targets = targetsInput;
    const struct SceneInputPosition *starts = startsInput;
    struct SceneInputWidget *widget;
    const u32 *enabled;
    u32 widgetOffset;
    const u32 *localFrames;
    s32 compare;
    u32 step;
    localFrames = frames;
    widget = pair->widgets;
    {
        step = 0;
        widgetOffset = step;
    }
    enabled = enabledInput;
    do {
        if (!*enabled)
            widget->flags |= 1;
        else {
            struct SceneInputPosition *position;
            u32 targetOffset;
            widget->flags &= ~1;
            Anim_LoadKeyframe(widget, pair, (u16)*localFrames, 1);
            position = (struct SceneInputPosition *)&pair->widgets[0].position;
            targetOffset = widgetOffset;
            position = (void *)((u8 *)position + targetOffset);
            {
                s32 lowIndex = i;
                targetOffset = lowIndex * 8;
            }
            *position = *starts;
            {
                struct SceneInputPosition *targetPosition = (void *)pair->targets;
                targetPosition = (void *)((u8 *)targetPosition + targetOffset);
                *targetPosition = *targets;
            }
        }
        {
            step = 8;
            targets = (void *)((const u8 *)targets + step);
        }
        starts++;
        localFrames++;
        widget++;
        {
            step = widgetOffset;
            step += 0x60;
            widgetOffset = step;
        }
        enabled++;
        {
            u32 step = 1;
            i += step;
        }
        {
            s32 value = i;
            compare = value;
        }
    } while (compare <= 1);
    {
        u32 zero = 0;
        u16 *frame = &pair->frame;
        *frame = zero;
    }
    {
        u16 *field = &pair->duration;
        *field = duration;
    }
}
