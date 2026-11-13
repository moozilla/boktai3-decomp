#include "dialogue_transition.h"
#include "input.h"

void DialogueTransition_ApplyEntryBounds(struct DialogueTransition *transition)
{
    int i = transition->index;
    MessageWindow_SetBounds(transition->x[i], transition->y[i], transition->width[i],
                            transition->height[i]);
}

void DialogueTransition_SetState(struct DialogueTransition *transition, u8 v)
{
    transition->state = v;
    transition->entered = 1;
    transition->ticks = 0;
}

u8 DialogueTransition_ConsumeStateEntry(struct DialogueTransition *transition)
{
    u32 r;
    if (transition->entered == 0) {
        r = 0;
    } else {
        transition->entered = 0;
        r = 1;
    }
    return r;
}

void DialogueTransition_UpdateFadeIn(struct DialogueTransition *transition)
{
    if (DialogueTransition_ConsumeStateEntry(transition)) {
        Bg_ClearTilemap(0);
        DialogueTransition_ApplyEntryBounds(transition);
        MessageWindow_ShowMessage(transition->index);
    }
    if (transition->ticks >= transition->fadeIn[transition->index]) {
        gBgFadeLevel = 64;
        DialogueTransition_SetState(transition, 1);
    } else {
        int result = Div(transition->ticks << 6, transition->fadeIn[transition->index]);
        gBgFadeLevel = result;
    }
}

void DialogueTransition_UpdateHold(struct DialogueTransition *transition)
{
    if (DialogueTransition_ConsumeStateEntry(transition))
        gBgFadeLevel = 64;
    if (transition->ticks >= (u32)transition->delays[transition->index])
        DialogueTransition_SetState(transition, 2);
}

void DialogueTransition_UpdateFadeOut(struct DialogueTransition *transition)
{
    DialogueTransition_ConsumeStateEntry(transition);
    if (transition->ticks >= transition->fadeOut[transition->index]) {
        gBgFadeLevel = 0;
        if (transition->index == transition->lastIndex) {
            MessageWindow_Clear();
            DialogueTransition_SetState(transition, 4);
            if (transition->completionProcedure)
                Script_Call(transition->completionProcedure, 0);
            if (transition->flags & 2)
                Task_Kill(transition);
        } else {
            transition->index++;
            DialogueTransition_SetState(transition, 0);
        }
    } else
        gBgFadeLevel = 64 - Div(transition->ticks << 6, transition->fadeOut[transition->index]);
}

void DialogueTransition_UpdateSkipFadeOut(struct DialogueTransition *transition)
{
    DialogueTransition_ConsumeStateEntry(transition);
    if (transition->skipProcedure) {
        if (transition->ticks > 15) {
            u32 *brightness = &gBgFadeLevel;
            int zero = 0;
            *brightness = zero;
            Script_Call(transition->skipProcedure, 0);
            transition->skipProcedure = zero;
            Task_Kill(transition);
        } else
            gBgFadeLevel = 64 - Div(transition->ticks << 6, 16);
    }
}

int DialogueTransition_OnTaskUpdate(struct DialogueTransition *transition)
{
    void *callback = gUnk_086056D8[transition->state];
    if (callback)
        Callback_InvokeActor(transition, callback);
    {
        u16 *reg = &gBgPaletteBuffer;
        int value = 0x1084;
        *reg = value;
    }
    if ((gPadState[1] & 9) && transition->skipProcedure && transition->state <= 2)
        DialogueTransition_SetState(transition, 3);
    transition->ticks++;
    return 0;
}

int DialogueTransition_OnTaskDestroy(struct DialogueTransition *transition)
{
    gTransition = 0;
    MessageWindow_Clear();
    if (transition->flags & 1)
        gDispCntShadow = transition->savedDisplayControl;
    return 0;
}

int DialogueTransition_LoadFromScript(struct DialogueTransition *transition)
{
    int zero = 0;
    transition->state = zero;
    transition->entered = zero;
    transition->ticks = zero;
    transition->index = Script_GetKeywordValue('p', 0);
    transition->parameter = zero;
    if (!Script_SeekToKeyword('s'))
        return -1;
    transition->script = Script_GetTextSourcePc();
    if (!transition->script)
        return -1;
    if (Script_SeekToKeyword('t')) {
        int i;
        transition->entries = (void *)sub_08227E9C();
        Script_SetPc(transition->entries);
        for (i = 0; i <= 63; i++) {
            transition->delays[i] = Script_GetValue();
            transition->x[i] = Script_GetValue();
            transition->y[i] = Script_GetValue();
            transition->width[i] = Script_GetValue();
            transition->height[i] = Script_GetValue();
            transition->fadeIn[i] = Script_GetValue();
            transition->fadeOut[i] = Script_GetValue();
            if (i >= transition->index && transition->delays[i] == -1) {
                transition->lastIndex = i - 1;
                break;
            }
        }
    }
    {
        int procedure = Script_GetKeywordValue('e', 0);
        zero = 0;
        transition->completionProcedure = procedure;
    }
    transition->skipProcedure = Script_GetKeywordValue('c', 0);
    MessageWindow_SetMessageTable(transition->script);
    MessageWindow_SetDisplayMode(1);
    MessageWindow_SetPalette(0x539C);
    MessageWindow_ShowMessage(transition->index);
    DialogueTransition_ApplyEntryBounds(transition);
    gBgFadeColor = 0x1084;
    gBgFadeBankMask = 0x8000;
    gBgFadeLevel = zero;
    DialogueTransition_SetState(transition, 0);
    return 0;
}

int DialogueTransition_Initialize(struct DialogueTransition *transition)
{
    gTransition = (void *)transition;
    transition->flags = Script_GetKeywordValue('f', 0);
    if (transition->flags & 1) {
        int previous = gDispCntShadow;
        transition->savedDisplayControl = previous;
        {
            int value = previous & ~0xE00;
            int bits = 0x100;
            value |= bits;
            gDispCntShadow = value;
        }
    }
    return DialogueTransition_LoadFromScript(transition);
}

struct DialogueTransition *DialogueTransition_CreateFromScript(void)
{
    struct DialogueTransition *transition;
    if (gTransition) {
        DialogueTransition_LoadFromScript((void *)gTransition);
        return (void *)gTransition;
    }
    transition = Task_Create(8, sizeof(*transition));
    if (transition) {
        Task_SetCallbacks(transition, (void (*)(void *))DialogueTransition_OnTaskUpdate,
                          (void (*)(void *))DialogueTransition_OnTaskDestroy);
        if (DialogueTransition_Initialize(transition) < 0) {
            Task_Kill(transition);
            return 0;
        }
    }
    return transition;
}
