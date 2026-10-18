#include "scene_input_grid.h"
#ifdef NONMATCHING
// fuzzy: 91.3%
// Register allocation around zero, step and widget offset still differs (192/180 bytes).
void SceneInput_BeginPairTransition(struct SceneInputPair *pairInput, const u32 *enabledInput,
                                    const struct SceneInputPosition *startsInput,
                                    const struct SceneInputPosition *targetsInput,
                                    const u32 *volatile frames, const u32 *unused, u32 duration)
{
    struct SceneInputPair *pair = pairInput;
    s32 i = 0;
    const struct SceneInputPosition *targets = targetsInput;
    const struct SceneInputPosition *starts = startsInput;
    struct SceneInputWidget *widget;
    const u32 *enabled;
    volatile u32 widgetOffset;
    const u32 *volatile localFrames;
    s32 compare;
    {
        const u32 *incomingFrames = frames;
        localFrames = incomingFrames;
    }
    widget = pair->widgets;
    {
        u32 zero = 0;
        widgetOffset = zero;
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
            {
                u32 offset = widgetOffset;
                position = (void *)((u8 *)position + offset);
            }
            {
                s32 lowIndex = i;
                targetOffset = lowIndex * 8;
            }
            {
                const u32 *source = (const u32 *)starts;
                u32 low;
                u32 high;
                low = source[0];
                high = source[1];
                ((u32 *)position)[0] = low;
                ((u32 *)position)[1] = high;
            }
            position = (void *)pair->targets;
            position = (void *)((u8 *)position + targetOffset);
            {
                const u32 *source = (const u32 *)targets;
                u32 low;
                u32 high;
                low = source[0];
                high = source[1];
                ((u32 *)position)[0] = low;
                ((u32 *)position)[1] = high;
            }
        }
        {
            u32 step = 8;
            targets = (void *)((const u8 *)targets + step);
        }
        starts++;
        {
            const u32 *cursor = localFrames;
            cursor++;
            localFrames = cursor;
        }
        widget++;
        {
            u32 offset = widgetOffset;
            offset += 0x60;
            widgetOffset = offset;
        }
        enabled++;
        {
            u32 step = 1;
            i += step;
        }
        compare = i;
    } while (compare <= 1);
    {
        u32 zero = 0;
        u16 *frame;
        frame = &pair->frame;
        *frame = zero;
    }
    {
        u16 *field = &pair->duration;
        *field = duration;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneInput_BeginPairTransition);
#endif
