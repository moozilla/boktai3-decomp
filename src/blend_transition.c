#include "blend_transition.h"

int BlendTransition_Update(struct BlendTransition *transition)
{
    int first;
    Video_SetBlendControl(transition->mode, transition->source, transition->destination);
    first =
        transition->initial[0] + Div(transition->delta[0] * transition->time, transition->duration);
    if (transition->mode == 1)
        Video_SetBlendAlpha(first,
                            transition->initial[1] +
                                Div(transition->delta[1] * transition->time, transition->duration));
    else
        Video_SetBlendBrightness(first);
    if (transition->time >= transition->duration)
        Task_Kill(transition);
    transition->time++;
    return 0;
}

u32 BlendTransition_Destroy(void)
{
    return 0;
}

/* Clear in reverse order and return the cursor after the final store. */

static inline u8 *BlendFade_ClearCoefficients(u8 *first)
{
    s32 zero = 0;
    u8 *cursor = first + 1;
    do {
        *cursor = zero;
        cursor--;
    } while ((s32)cursor >= (s32)first);
    first = cursor;
    return first;
}

// confidence=low: reads a blend fade from the script: mode 'm', start coefficients 'i', their

// change 'a', targets 's' and 'd' and duration 't' (default 60), then applies the start values.

s32 BlendTransition_InitializeFromScript(struct BlendTransition *transition)
{
    u8 *start;
    u8 *delta;
    u8 *s;
    s32 i;
    transition->mode = Script_GetKeywordValue('m', 1);
    if (Script_SeekToKeyword('i')) {
        start = transition->initial;
        delta = transition->delta;
        s = start;
    read_start: {
        s32 value = (s32)Script_GetPc();
        if (value)
            value = Script_GetValue();
        *s = value;
        s++;
    }
        if ((s32)s <= (s32)(start + 1))
            goto read_start;
    } else {
        start = transition->initial;
        delta = transition->delta;
        s = BlendFade_ClearCoefficients(start);
    }
    if (Script_SeekToKeyword('a')) {
        i = 0;
        s = delta;
        do {
            u8 coefficient;
            s32 value = (s32)Script_GetPc();
            if (value)
                value = Script_GetValue() - start[i];
            coefficient = value;
            *s = coefficient;
            s++;
            i++;
        } while (i <= 1);
    } else {
        s = BlendFade_ClearCoefficients(delta);
    }
    transition->source = Script_GetKeywordValue('s', 0);
    transition->destination = Script_GetKeywordValue('d', 0);
    transition->duration = Script_GetKeywordValue('t', 60);
    transition->time = 0;
    if (transition->duration == 0)
        transition->duration = 60;
    Video_SetBlendControl(transition->mode, transition->source, transition->destination);
    if (transition->mode == 1)
        Video_SetBlendAlpha(transition->initial[0], transition->initial[1]);
    else
        Video_SetBlendBrightness(transition->initial[0]);
    return 0;
}

struct BlendTransition *BlendTransition_Create(void)
{
    struct BlendTransition *transition;
    transition = Task_Create(11, sizeof(*transition));
    if (transition != 0) {
        Task_SetCallbacks(transition, (void *)BlendTransition_Update,
                          (void *)BlendTransition_Destroy);
        if (BlendTransition_InitializeFromScript(transition) < 0) {
            Task_Kill(transition);
            return 0;
        }
    }
    return transition;
}

void CircleWindow_InitManager(void)
{
    gCircleWindow = 0;
}
