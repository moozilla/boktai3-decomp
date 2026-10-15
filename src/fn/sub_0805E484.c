#include "scene_input_grid.h"
#ifdef NONMATCHING
// fuzzy: 91.3%
// Register allocation around zero, step and widget offset still differs (192/180 bytes).
// MATCHING_CONCESSION: register-pinning
#define SceneInput_BeginPairTransition sub_0805E484
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
    register s32 compare asm("r3");
    asm("" : "+r"(starts), "+r"(targets) : "r"(i));
    {
        register const u32 *incomingFrames asm("r3") = frames;
        asm("" ::"r"(incomingFrames));
        localFrames = incomingFrames;
    }
    widget = pair->widgets;
    asm("" ::"r"(widget));
    {
        u32 zero = 0;
        asm("" : "+l"(zero)::"r0", "r2", "r3");
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
            position = &pair->widgets[0].position;
            asm("" ::"r"(position));
            {
                register u32 offset asm("r3") = widgetOffset;
                asm("" ::"r"(offset));
                position = (void *)((u8 *)position + offset);
            }
            {
                s32 lowIndex = i;
                asm("" : "+l"(lowIndex)::"r0", "r1", "r3");
                targetOffset = lowIndex * 8;
            }
            {
                const u32 *source = (const u32 *)starts;
                register u32 low asm("r0");
                register u32 high asm("r1");
                asm("" : "+l"(source)::"r0", "r1");
                low = source[0];
                high = source[1];
                asm("" ::"r"(low), "r"(high), "l"(source));
                ((u32 *)position)[0] = low;
                ((u32 *)position)[1] = high;
            }
            position = (void *)pair->targets;
            asm("" ::"r"(position));
            position = (void *)((u8 *)position + targetOffset);
            {
                const u32 *source = (const u32 *)targets;
                register u32 low asm("r0");
                register u32 high asm("r1");
                asm("" : "+l"(source)::"r0", "r1");
                low = source[0];
                high = source[1];
                asm("" ::"r"(low), "r"(high), "l"(source));
                ((u32 *)position)[0] = low;
                ((u32 *)position)[1] = high;
            }
        }
        {
            u32 step = 8;
            asm("" : "+l"(step)::"r0", "r1", "r2", "r3");
            targets = (void *)((const u8 *)targets + step);
        }
        starts++;
        {
            register const u32 *cursor asm("r3") = localFrames;
            cursor++;
            asm("" ::"r"(cursor));
            localFrames = cursor;
        }
        widget++;
        {
            u32 offset = widgetOffset;
            asm("" : "+l"(offset)::"r0", "r1", "r2", "r3");
            offset += 0x60;
            asm("" : : "l"(offset) : "r0", "r1", "r2", "r3");
            widgetOffset = offset;
        }
        enabled++;
        {
            register u32 step asm("r0") = 1;
            asm("" : "+r"(step));
            i += step;
        }
        compare = i;
        asm("" ::"r"(compare));
    } while (compare <= 1);
    {
        register u32 zero asm("r1") = 0;
        register u16 *frame asm("r0");
        asm("" ::"r"(zero));
        frame = &pair->frame;
        asm("" : "+r"(frame));
        *frame = zero;
    }
    {
        register u16 *field asm("r1") = &pair->duration;
        asm("" ::"r"(field));
        *field = duration;
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0805E484);
#endif
