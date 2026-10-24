// MATCHING_CONCESSION: register-pinning
#include "scene_input_grid.h"
#ifdef NONMATCHING
// fuzzy: 95.5%
// Register allocation around zero, step and widget offset still differs (192/180 bytes).
void SceneInput_BeginPairTransition(struct SceneInputPair *pairInput, const u32 *enabledInput,
                                    const struct SceneInputPosition *startsInput,
                                    const struct SceneInputPosition *targetsInput,
                                    const u32 *volatile frames, const u32 *unused, u32 duration)
{
    register struct SceneInputPair *pair asm("r5") = pairInput;
    register s32 i asm("r10") = 0;
    register const struct SceneInputPosition *targets asm("r9") = targetsInput;
    register const struct SceneInputPosition *starts asm("r8") = startsInput;
    register struct SceneInputWidget *widget asm("r4");
    register const u32 *enabled asm("r6");
    volatile u32 widgetOffset;
    const u32 *volatile localFrames;
    s32 compare;
    {
        register const u32 *incomingFrames asm("r3") = frames;
        localFrames = incomingFrames;
    }
    widget = pair->widgets;
    {
        u32 zero = 0;
        asm("" : "+l"(zero) : : "r0", "r2", "r3", "r6");
        widgetOffset = zero;
    }
    enabled = enabledInput;
    do {
        if (!*enabled)
            widget->flags |= 1;
        else {
            register struct SceneInputPosition *position asm("r2");
            register u32 targetOffset asm("r3");
            widget->flags &= ~1;
            Anim_LoadKeyframe(widget, pair, (u16)*localFrames, 1);
            position = (struct SceneInputPosition *)&pair->widgets[0].position;
            {
                register u32 offset asm("r3") = widgetOffset;
                position = (void *)((u8 *)position + offset);
            }
            {
                s32 lowIndex = i;
                asm("" : "+l"(lowIndex) : : "r0", "r1", "r3");
                targetOffset = lowIndex * 8;
            }
            {
                const struct SceneInputPosition *source = starts;
                u32 low;
                u32 high;
                asm("" : "+l"(source) : : "r0", "r1");
                low = ((const u32 *)source)[0];
                high = ((const u32 *)source)[1];
                ((u32 *)position)[0] = low;
                ((u32 *)position)[1] = high;
            }
            position = (void *)pair->targets;
            position = (void *)((u8 *)position + targetOffset);
            {
                const struct SceneInputPosition *source = targets;
                u32 low;
                u32 high;
                asm("" : "+l"(source) : : "r0", "r1");
                low = ((const u32 *)source)[0];
                high = ((const u32 *)source)[1];
                ((u32 *)position)[0] = low;
                ((u32 *)position)[1] = high;
            }
        }
        {
            u32 step = 8;
            asm("" : : "l"(step) : "r0", "r1", "r2", "r3");
            targets = (void *)((const u8 *)targets + step);
        }
        starts++;
        {
            register const u32 *cursor asm("r3") = localFrames;
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
            register u32 step asm("r0") = 1;
            asm("" : : "r"(step));
            i += step;
        }
        {
            register s32 value asm("r3") = i;
            asm("" : : "r"(value));
            compare = value;
        }
    } while (compare <= 1);
    {
        register u32 zero asm("r1") = 0;
        register u16 *frame asm("r0") = &pair->frame;
        asm("" : "+r"(frame) : "r"(zero));
        *frame = zero;
    }
    {
        register u16 *field asm("r1") = &pair->duration;
        asm("" : : "r"(field));
        *field = duration;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", SceneInput_BeginPairTransition);
#endif
