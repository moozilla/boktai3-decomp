#include "scrolling_dialogue.h"
#include "input.h"

void ScrollingDialogue_ClearTextArea(struct ScrollingDialogue *dialogue)
{
    struct MessagePlaybackState *text = &dialogue->text;
    TextWindow_BlankRect(0, 18, 32, 2);
    BackgroundMap_ClearTextRect(0, 18, 32, 2);
    TextState_SetBounds(text, 0, 18, 32, 2);
}

int ScrollingDialogue_ResetPlayback(struct ScrollingDialogue *dialogue, int clear)
{
    struct MessagePlaybackState *text = &dialogue->text;
    int zero;
    s32 *row;
    if (clear)
        ScrollingDialogue_ClearTextArea(dialogue);
    row = &dialogue->row;
    zero = 0;
    *row = zero;
    dialogue->column = zero;
    dialogue->ticks = zero;
    dialogue->scrolling = zero;
    dialogue->delay = 30;
    dialogue->blocked = zero;
    dialogue->wait = zero;
    text->pc = dialogue->script;
    text->stream = zero;
    text->wrap = 2;
    TextState_Reset(text);
}

void ScrollingDialogue_UpdatePlayback(struct ScrollingDialogue *dialogue)
{
    struct MessagePlaybackState *text = &dialogue->text;
    int advance;
    const u8 *cursor;
    advance = 0;
    if (dialogue->scrolling) {
        if (!dialogue->delay) {
            if (gPadState[0] & 4)
                dialogue->ticks = (dialogue->ticks & ~3) + 4;
            else
                dialogue->ticks++;
            advance = 1;
        } else
            dialogue->delay--;
    } else {
        dialogue->ticks = (dialogue->ticks & ~7) + 8;
        advance = 1;
    }
    if (dialogue->ticks & 7)
        goto done;
    if (dialogue->scrolling && !dialogue->delay && dialogue->blocked)
        dialogue->blocked = 0;
    if (!dialogue->wait && !dialogue->scrolling && dialogue->column == 31 &&
        (dialogue->ticks & 15) == 8) {
        {
            const u8 *next = TextState_GetActiveStream(text);
            if (next) {
                int c = (s8)*next;
                if (c < 0) {
                    dialogue->ticks -= 8;
                    dialogue->scrolling = 1;
                    dialogue->blocked = 1;
                }
            }
        }
    }
    if (dialogue->blocked)
        goto done;
    if (!advance)
        goto done;
    TextWindow_BlankRect(dialogue->row, 18, 1, 2);
    BackgroundMap_ClearTextRect(dialogue->row, 18, 1, 2);
    dialogue->row++;
    cursor = TextState_GetActiveStream(text);
    if (!cursor)
        goto wrap;
tags:
    if (*cursor != '<')
        goto render;
    TextState_StepPlayback(text);
    if (!text->hold)
        goto render;
    cursor = TextState_GetActiveStream(text);
    if (text->stream)
        goto tags;
    if (*cursor)
        goto tags;
    dialogue->wait = 8;
    goto tags;
render:
    if ((s8)*cursor >= 0) {
        TextState_SetBounds(&dialogue->text, dialogue->column, 18, 1, 2);
        dialogue->column++;
        if (dialogue->wait > 0) {
            TextState_DrawNarrowGlyph(text, 0);
            dialogue->wait--;
            if (dialogue->wait > 0)
                goto wrap;
            text->pc = dialogue->script;
            dialogue->wait = 0;
            goto wrap;
        }
        if (TextState_StepPlayback(text))
            dialogue->wait = 8;
        goto wrap;
    }
    if (!(dialogue->column & 1)) {
        if ((dialogue->ticks & 15) == 0)
            goto paintWide;
        goto wrap;
    }
    if ((dialogue->ticks & 15) != 8)
        goto wrap;
paintWide:
    if (dialogue->wait > 0) {
        TextState_DrawNarrowGlyph(text, 0);
        dialogue->column++;
        dialogue->wait--;
        if (dialogue->wait > 0)
            goto wrap;
        text->pc = dialogue->script;
        dialogue->wait = 0;
    } else {
        TextState_SetBounds(&dialogue->text, dialogue->column, 18, 2, 2);
        dialogue->column += 2;
        if (TextState_StepPlayback(text))
            dialogue->wait = 8;
    }
wrap:
    if (dialogue->column > 31) {
        if (!dialogue->scrolling)
            dialogue->scrolling = 1;
        dialogue->column -= 32;
    }
    if (dialogue->row > 31)
        dialogue->row -= 32;
done:
    if (!dialogue->scrolling)
        ScrollingDialogue_UpdatePlayback(dialogue);
}

int ScrollingDialogue_Show(struct ScrollingDialogue *dialogue)
{
    u8 *active = &dialogue->active;
    int result;
    if (!*active) {
        dialogue->ticks = 0;
        ScrollingDialogue_ResetPlayback(dialogue, 1);
        gVBlankDialogueState = 1;
        *active = 1;
        result = 0;
    } else {
        result = -1;
    }
    return result;
}

int ScrollingDialogue_Hide(struct ScrollingDialogue *dialogue)
{
    u8 *active = &dialogue->active;
    int result;
    if (*active) {
        ScrollingDialogue_ClearScript();
        gVBlankDialogueState = 0;
        *active = 0;
        result = 0;
    } else {
        result = -1;
    }
    return result;
}

int ScrollingDialogue_OnTaskUpdate(struct ScrollingDialogue *dialogue)
{
    struct MessagePlaybackState *text;
    if (dialogue->showRequest) {
        ScrollingDialogue_Show(dialogue);
        dialogue->showRequest = 0;
    }
    if (dialogue->hideRequest) {
        ScrollingDialogue_Hide(dialogue);
        dialogue->hideRequest = 0;
    }
    if (dialogue->active) {
        text = &dialogue->text;
        TextState_ClearSelectedChoices(text);
        ScrollingDialogue_UpdatePlayback(dialogue);
        if (dialogue->scrolling)
            gDialogueScrollOffset = dialogue->ticks;
        else
            gDialogueScrollOffset = 0;
        dialogue->frames++;
        TextState_RunSelectedProcedures(text);
    }
    return 0;
}

int ScrollingDialogue_OnTaskDestroy(struct ScrollingDialogue *dialogue)
{
    u8 *active = &dialogue->active;
    if (*active)
        *active = 0;
    gDialogueScrollOffset = 0;
    gScrollingDialogue = 0;
    return 0;
}

int ScrollingDialogue_Init(struct ScrollingDialogue *dialogue)
{
    struct MessagePlaybackState *text;
    int zero;
    gScrollingDialogue = (void *)dialogue;
    text = &dialogue->text;
    {
        u8 *active = &dialogue->active;
        zero = 0;
        *active = zero;
    }
    dialogue->showRequest = zero;
    dialogue->hideRequest = zero;
    dialogue->frames = zero;
    dialogue->row = zero;
    dialogue->column = zero;
    TextState_Init(text, 0, 18, 2, 2);
    text->unknown14 = zero;
    dialogue->script = gUnk_086056D0;
    text->pc = gUnk_086056D0;
    ScrollingDialogue_ResetPlayback(dialogue, 0);
    text->spacing = zero;
    text->skip = 1;
    dialogue->ticks = zero;
    gDialogueScrollOffset = zero;
    return 0;
}

struct ScrollingDialogue *ScrollingDialogue_Create(void)
{
    struct ScrollingDialogue *dialogue;
    if (gScrollingDialogue != 0)
        return (void *)gScrollingDialogue;
    dialogue = Task_Create(0x3, 0x19c);
    if (dialogue != 0) {
        Task_SetCallbacks(dialogue, (void (*)(void *))ScrollingDialogue_OnTaskUpdate,
                          (void (*)(void *))ScrollingDialogue_OnTaskDestroy);
        if (ScrollingDialogue_Init(dialogue) < 0) {
            Task_Kill(dialogue);
            return 0;
        }
    }
    return dialogue;
}

int ScrollingDialogue_SetScript(const u8 *script)
{
    struct ScrollingDialogue *dialogue = (void *)gScrollingDialogue;
    int result;
    if (dialogue) {
        dialogue->script = script;
        ScrollingDialogue_ResetPlayback(dialogue, 1);
        result = 0;
    } else {
        result = -1;
    }
    return result;
}

u32 ScrollingDialogue_ClearScript(void)
{
    return ScrollingDialogue_SetScript(gUnk_086056D0);
}

int ScrollingDialogue_SetNumberArgument(int index, int value)
{
    struct ScrollingDialogue *dialogue = (void *)gScrollingDialogue;
    int result;
    if (dialogue) {
        TextState_SetNumber(&dialogue->text, index, value);
        result = 0;
    } else {
        result = -1;
    }
    return result;
}

s32 ScrollingDialogue_SetNumberArgumentFromScript(void)
{
    s32 index = Script_GetValue();
    return ScrollingDialogue_SetNumberArgument(index, Script_GetValue());
}

s32 ScrollingDialogue_SetStringArgument(u32 a, u32 b)
{
    struct ScrollingDialogue *dialogue = (void *)gScrollingDialogue;
    s32 r;
    if (dialogue == 0)
        r = -1;
    else
        r = TextState_SetString(&dialogue->text, a, b);
    return r;
}

s32 ScrollingDialogue_SetStringFromScript(void)
{
    s32 i = Script_GetValue();
    return ScrollingDialogue_SetStringArgument(i, (u32)Script_GetTextSourcePc());
}

s32 ScrollingDialogue_SetStringReferenceFromScript(void)
{
    s32 i = Script_GetValue();
    return ScrollingDialogue_SetStringArgument(i, sub_08227E9C());
}

s32 ScrollingDialogue_SetChoicesFromScript(void)
{
    struct ScrollingDialogue *dialogue = (void *)gScrollingDialogue;
    if (dialogue != 0) {
        struct MessagePlaybackState *text = &dialogue->text;
        text->choiceCount = 0;
        if (Script_SeekToKeyword(0x70)) {
            while (Script_GetPc()) {
                text->choices[text->choiceCount] = Script_GetValue();
                text->choiceCount++;
            }
            return 0;
        }
    }
    return -1;
}

s32 ScrollingDialogue_RequestShow(void)
{
    s32 ret;

    struct ScrollingDialogue *dialogue = (void *)gScrollingDialogue;
    if (dialogue) {
        dialogue->showRequest = 1;
        ret = 0;
    } else {
        ret = -1;
    }
    return ret;
}

int ScrollingDialogue_RequestHide(void)
{
    struct ScrollingDialogue *dialogue = (void *)gScrollingDialogue;
    int result;
    if (dialogue) {
        dialogue->hideRequest = 1;
        result = 0;
    } else {
        result = -1;
    }
    return result;
}

s32 ScrollingDialogue_HideImmediately(void)
{
    s32 r;
    if (gScrollingDialogue != 0) {
        ScrollingDialogue_Hide((void *)gScrollingDialogue);
        r = 0;
    } else
        r = -1;
    return r;
}
