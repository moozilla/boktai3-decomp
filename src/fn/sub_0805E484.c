// MATCHING_CONCESSION: register-pinning
#include "scene_input_grid.h"
void SceneInput_BeginPairTransition(struct SceneInputPair *pairInput, const u32 *enabledInput,
                                    const struct SceneInputPosition *startsInput,
                                    const struct SceneInputPosition *targetsInput,
                                    const u32 *volatile frames, const u32 *unused, u32 duration)
{
    register struct SceneInputPair *pair asm("r5") = pairInput;
    register s32 i asm("r10") = 0;
    const struct SceneInputPosition *targets = targetsInput;
    const struct SceneInputPosition *starts = startsInput;
    register struct SceneInputWidget *widget asm("r4");
    const u32 *enabled;
    volatile u32 widgetOffset;
    const u32 *volatile localFrames;
    s32 compare;
    u32 step;
    {
        targetsInput = (const struct SceneInputPosition *)frames;
        localFrames = (const u32 *)targetsInput;
    }
    widget = pair->widgets;
    {
        step = 0;
        asm("" : "+l"(step) : : "r0", "r2", "r3", "r6", "ip");
        widgetOffset = step;
    }
    enabled = enabledInput;
    do {
        if (!*enabled)
            widget->flags |= 1;
        else {
            struct SceneInputPosition *position;
            register u32 targetOffset asm("r3");
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
            position = (void *)pair->targets;
            position = (void *)((u8 *)position + targetOffset);
            *position = *targets;
        }
        {
            step = 8;
            targets = (void *)((const u8 *)targets + step);
        }
        starts++;
        targetsInput = (const struct SceneInputPosition *)localFrames;
        targetsInput = (const struct SceneInputPosition *)((const u32 *)targetsInput + 1);
        localFrames = (const u32 *)targetsInput;
        widget++;
        {
            step = widgetOffset;
            step += 0x60;
            widgetOffset = step;
        }
        enabled++;
        {
            register u32 step asm("r0") = 1;
            asm("" : : "r"(step));
            i += step;
        }
        {
            s32 value = i;
            compare = value;
        }
    } while (compare <= 1);
    {
        u32 zero = 0;
        register u16 *frame asm("r0") = &pair->frame;
        *frame = zero;
    }
    {
        register u16 *field asm("r1") = &pair->duration;
        *field = duration;
    }
}
