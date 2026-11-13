#include "message_window.h"
#include "input.h"
#include "display_sequence_inline.h"

static inline s32 MessageWindow_GetCharacterDelay(void)
{
    return (s16)gGameState->unk12;
}

static inline void MessageWindow_SetPlayback(struct MessageWindow *window, void *callback)
{
    window->callback = callback;
}

u32 MessageWindow_IsCreated(void)
{
    u32 x = (u32)gMessageWindow;
    if (x != 0)
        x = 1;
    return x;
}

int MessageWindow_LoadPalette(struct MessageWindow *window, int palette)
{
    void *data = ScriptProc_Start(0x92b3, (u16)palette);
    int result;
    if (data) {
        window->palette = palette;
        window->paletteData = data;
        result = 0;
    } else
        result = -1;
    return result;
}

void MessageWindow_ApplyPalette(struct MessageWindow *window)
{
    CpuSet(window->paletteData->banks[15], gBgPaletteBank15, 0x04000008);
}

int MessageWindow_UpdateSpeakerPortrait(struct MessageWindow *window)
{
    struct MessagePlaybackState *state = &window->state;
    if (state->speaker == -1) {
        window->portrait.flags |= 1;
    } else {
        struct DisplaySequenceState *f;
        struct DisplaySequenceState *initial;
        struct ContactSwarmSprite *r;
        const u16 *word;
        window->portrait.flags &= ~1;
        {
            switch ((s8)(state->speaker - 4)) {
                case 26:
                    AnimData_SelectPaletteBank(window->portraitGraphics, 506);
                    break;
                case 16:
                case 21:
                    AnimData_SelectPaletteBank(window->portraitGraphics, 504);
                    break;
                case 0:
                case 1:
                case 9:
                    AnimData_SelectPaletteBank(window->portraitGraphics, 505);
                    break;
                case 15:
                    AnimData_SelectPaletteBank(window->portraitGraphics, 507);
                    break;
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 17:
                case 18:
                case 19:
                case 20:
                case 22:
                case 23:
                case 24:
                case 25:
                default:
                    AnimData_SelectPaletteBank(window->portraitGraphics, 503);
                    break;
            }
        }
        initial = &window->portraitSequence;
        Anim_SetSequence(initial, window->portraitResource, (u16)state->speaker, 0, 0);
        r = &window->portrait;
        f = initial;
        word = f->commands + f->index;
        r->frame = *word >> 6;
        {
            union DisplaySequenceControl control;
            u32 bit, first;
            control.raw = f->flags;
            bit = 1;
            first = (u8)(control.raw & bit);
            if (first != (((*word & 0x30) >> 4) & bit))
                r->flags |= 4;
            else
                r->flags &= ~4;
        }
        {
            union DisplaySequenceControl control;
            u32 bit, first;
            control.raw = f->flags;
            bit = 2;
            first = (u8)(control.raw & bit);
            if (first != (((*word & 0x30) >> 4) & bit))
                r->flags |= 8;
            else
                r->flags &= ~8;
        }
        if (++f->elapsed >= f->duration) {
            DisplaySequence_SelectNext(f);
            word = f->commands + f->index;
            DisplaySequence_UpdateWeight(f, word);
            if (f->duration == 0)
                f->duration = 1;
        }
    }
    window->previousSpeaker = (u8)state->speaker;
}

int MessageWindow_SetDisplayMode(int mode)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    if (!window)
        return 0;
    window->displayMode = mode;
    if (!(u8)mode) {
        MessageWindow_SetPlayback(window, MessageWindow_UpdateNormalPlayback);
        {
            u8 *mode = &window->inputDelay;
            *mode = 3;
        }
    } else {
        window->playbackCounter = 0;
        MessageWindow_SetPlayback(window, MessageWindow_UpdateImmediatePlayback);
    }
    {
        u8 *fresh = &window->state.complete;
        *fresh = 0;
        {
            struct MessagePlaybackState *state = &window->state;
            state->speaker = -1;
        }
    }
    return MessageWindow_UpdateSpeakerPortrait(window);
}

u32 MessageWindow_SetDisplayModeFromScript(void)
{
    return MessageWindow_SetDisplayMode(Script_GetValue());
}

int MessageWindow_Clear(void)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    int result;
    if (window) {
        goto run;
    }
    result = 0;
done:
    return result;

run:
    TextWindow_BlankRect(window->x, window->y, window->width, window->height);
    {
        void *fn = MessageWindow_ClearTextCompletion;
        void **dst;
        dst = &window->callback;
        *dst = fn;
    }
    window->widget.flags |= 1;
    {
        struct MessagePlaybackState *state = &window->state;
        state->speaker = -1;
    }
    window->portrait.flags |= 1;
    window->queuedMessage = 65535;
}

u32 MessageWindow_ClearFromScript(u8 *window)
{
    return MessageWindow_Clear();
}

int MessageWindow_SetMessageTable(void *script)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    if (window && script) {
        struct MessagePlaybackState *state = &window->state;
        int zero;
        window->messageTable = script;
        {
            u16 *index = &window->messageIndex;
            zero = 0;
            *index = zero;
        }
        state->pc = MessageWindow_GetStringAtOffset(window, zero);
        window->messageFrame = zero;
        {
            u16 *count = &window->messageCount;
            int one;
            one = 1;
            *count = one;
        }
        window->queuedMessage = zero;
        TextState_Reset(state);
        window->widget.flags |= 1;
    }
    return 0;
}

s32 MessageWindow_SetTimedMessageTable(void *script, u32 count, u32 frame, u16 *frames)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    if (window != 0) {
        struct MessagePlaybackState *state = &window->state;
        s32 i;
        window->messageTable = script;
        state->pc = MessageWindow_GetStringAtOffset(window, 0);
        window->messageFrame = frame;
        window->messageCount = count;
        if ((u16)count > 0x20)
            window->messageCount = 0x20;
        for (i = 0; i < window->messageCount; i++)
            window->messageFrames[i] = frames[i];
        window->queuedMessage = window->messageFrames[window->messageFrame];
        TextState_Reset(&window->state);
        window->widget.flags |= 1;
    }
    return 0;
}

u32 MessageWindow_SetMessageTableFromScript(void)
{
    u32 v;
    if (Script_SeekToKeyword(0x72) == 0)
        v = (u32)Script_GetPc();
    else
        v = (u32)Script_GetTextSourcePc();
    return MessageWindow_SetMessageTable((void *)v);
}

int MessageWindow_ShowMessage(int index)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    void *script;
    struct MessagePlaybackState *state;
    if (!window)
        return 0;
    script = window->messageTable;
    state = &window->state;
    if (!script)
        return 0;
    TextWindow_BlankRect(window->x, window->y, window->width, window->height);
    TextState_SetBounds(state, window->x, window->y, window->width, window->height);
    window->messageIndex = index;
    state->pc = MessageWindow_GetStringAtOffset(window, window->messageIndex);

    {
        u8 mode;
        {
            u8 *ptr;
            ptr = &window->displayMode;
            mode = *ptr;
        }
        if (!mode) {
            {
                void *fn = MessageWindow_UpdateNormalPlayback;
                void **dst;
                dst = &window->callback;
                *dst = fn;
            }
            {
                u8 *mode;
                mode = &window->inputDelay;
                *mode = 3;
            }
        } else {
            {
                u32 *counter;
                counter = &window->playbackCounter;
                {
                    u32 zero = 0;
                    *counter = zero;
                }
            }
            {
                void *fn = MessageWindow_UpdateImmediatePlayback;
                void **dst;
                dst = &window->callback;
                *dst = fn;
            }
        }
    }
    TextState_Reset(state);
    window->queuedMessage = 0xFFFF;
    window->widget.flags |= 1;
    return 0;
}

int MessageWindow_SetWaitFromScript(void)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    if (window && window->messageTable)
        window->queuedMessage = Script_GetValue();
    return 0;
}

s32 MessageWindow_SetTextCursor(u32 value)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    if (window) {
        struct MessagePlaybackState *state = &window->state;
        window->messageTable = 0;
        window->queuedMessage = 0;
        state->pc = (const u8 *)value;
        state->characterTimer = 0;
        state->stream = 0;
        window->widget.flags |= 1;
    }
    return 0;
}

int MessageWindow_SetBounds(int x, int y, int width, int height)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    int result;
    if (window)
        goto run;
    result = 0;
    goto done;
run: {
    TextWindow_BlankRect(window->x, window->y, window->width, window->height);
    window->x = x;
    window->y = y;
    window->width = width;
    window->height = height;
    /* This caller forwards the callee's r0; engine.h currently declares void. */
    result = ((s32(*)(void *, u32, u32, u32, u32))TextState_SetBounds)(
        &window->state, window->x, window->y, window->width, window->height);
}
done:
    return result;
}

int MessageWindow_SetBoundsFromScript(void)
{
    int x = Script_GetValue();
    int y = Script_GetValue();
    int width = Script_GetValue();
    int height = Script_GetValue();
    return MessageWindow_SetBounds(x, y, width, height);
}

int MessageWindow_SetNumberArgument(int a, int b)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    int result;
    if (window) {
        TextState_SetNumber(&window->state, a, b);
        result = 0;
    } else
        result = -1;
    return result;
}

u32 MessageWindow_SetNumberArgumentFromScript(void)
{
    s32 a = Script_GetValue();
    return MessageWindow_SetNumberArgument(a, Script_GetValue());
}

u32 MessageWindow_SetStringArgument(u32 argument, u32 text)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    u32 r;
    if (window == 0)
        r = 0;
    else
        r = TextState_SetString(&window->state, argument, text);
    return r;
}

int MessageWindow_SetStringArgumentFromScript(void)
{
    int kind = Script_GetValue();
    void *ref = Script_GetTextSourcePc();
    int result;
    if (ref) {
        int offset = Script_GetKeywordValue(0x69, 0);
        int id = Script_ParseStringRef(ref);
        MessageWindow_SetStringArgument(kind, (u32)Text_LookupString(id + offset));
        result = 0;
    } else
        result = -1;
    return result;
}

s32 MessageWindow_SetStringReferenceFromScript(void)
{
    s32 a = Script_GetValue();
    return MessageWindow_SetStringArgument(a, sub_08227E9C());
}

int MessageWindow_SetStringFromPc(void)
{
    int kind = Script_GetValue();
    void *ref = Script_GetPc();
    int result;
    if (ref) {
        int offset = Script_GetKeywordValue(0x69, 0);
        int id = Script_ParseStringRef(ref);
        MessageWindow_SetStringArgument(kind, (u32)Text_LookupString(id + offset));
        result = 0;
    } else
        result = -1;
    return result;
}

s32 MessageWindow_SetArgumentFromScript(void)
{
    s32 a = Script_GetValue();
    return MessageWindow_SetStringArgument(a, Script_GetValueSafe());
}

int MessageWindow_SetChoicesFromScript(void)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    struct MessagePlaybackState *vm;
    int result;
    if (!window)
        return 0;
    vm = &window->state;
    vm->choiceCount = 0;
    if (Script_SeekToKeyword(0x70)) {
        while (Script_GetPc()) {
            int value = Script_GetValue();
            vm->choices[vm->choiceCount] = value;
            vm->choiceCount++;
        }
        result = 0;
    } else
        result = -1;
    return result;
}

int MessageWindow_SetAutoAdvance(int mode, int step, int flags)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    if (!window)
        return 0;
    {
        u8 *dst = &window->autoAdvance;
        int zero;
        zero = 0;
        *dst = mode;
        window->autoAdvanceStep = step;
        window->autoAdvanceCounter = zero;
        window->skipFlags = flags;
    }
}

int MessageWindow_SetAutoAdvanceFromScript(void)
{
    int mode = Script_GetValue();
    int step = Script_GetValue();
    int flags = Script_GetKeywordValue(0x6d, mode != 0);
    return MessageWindow_SetAutoAdvance(mode, step, flags);
}

int MessageWindow_SetPalette(int palette)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    int result;
    if (window && MessageWindow_LoadPalette(window, palette) >= 0) {
        MessageWindow_ApplyPalette(window);
        result = 0;
    } else
        result = -1;
    return result;
}

u32 MessageWindow_SetPaletteFromScript(void)
{
    return MessageWindow_SetPalette(Script_GetValue());
}

int MessageWindow_OpenFromScript(void)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    struct MessagePlaybackState *state;
    if (!window)
        return -1;
    state = &window->state;
    if (Script_SeekToKeyword('s')) {
        void *script = Script_GetTextSourcePc();
        window->messageTable = script;
        if (!script)
            return -1;
        MessageWindow_SetMessageTable(script);
    }
    if (Script_SeekToKeyword('r')) {
        int x = Script_GetValue();
        int y = Script_GetValue();
        int width = Script_GetValue();
        int height = Script_GetValue();
        MessageWindow_SetBounds(x, y, width, height);
    }
    {
        s32 palette = Script_GetKeywordValue('c', window->palette);
        if (MessageWindow_LoadPalette(window, palette) < 0)
            return -1;
    }
    MessageWindow_ApplyPalette(window);
    window->messageIndex = Script_GetKeywordValue('i', window->messageIndex);
    state->spacing = Script_GetKeywordValue('m', state->spacing);
    state->characterDelay = Script_GetKeywordValue('w', (s16)gGameState->unk12);
    if (Script_SeekToKeyword('d')) {
        window->autoAdvance = Script_GetValue();
        window->autoAdvanceStep = Script_GetValue();
        window->skipFlags = 1;
    } else {
        window->autoAdvance = 0;
        window->autoAdvanceStep = 0;
        window->skipFlags = 0;
    }
    window->autoAdvanceCounter = 0;
    if (Script_SeekToKeyword('p')) {
        state->choiceCount = 0;
        while (Script_GetPc()) {
            state->choices[state->choiceCount] = Script_GetValue();
            state->choiceCount++;
        }
    }
    if (Script_SeekToKeyword('a')) {
        if (Script_GetValue())
            window->fastPlayback = 0;
    } else
        window->fastPlayback = 0;
}

int MessageWindow_SetPageAdvanceDelay(u32 delay)
{
    struct MessageWindow *window = (void *)gMessageWindow;
    int result;
    if (window) {
        window->pageAdvanceDelay = delay;
        window->pageAdvanceTimer = 0;
        result = 0;
    } else
        result = -1;
    return result;
}

u32 MessageWindow_SetPageAdvanceDelayFromScript(void)
{
    return MessageWindow_SetPageAdvanceDelay(Script_GetValue());
}

s32 MessageWindow_CheckExists(void)
{
    if (gMessageWindow == NULL)
        return -1;
    return 0;
}

const u8 *MessageWindow_GetStringAtOffset(struct MessageWindow *window, u32 i)
{
    return Text_LookupString(Script_ParseStringRef(window->messageTable) + i);
}

void MessageWindow_UpdatePageAdvance(struct MessageWindow *window)
{
    struct MessagePlaybackState *st = &window->state;
    s32 result = 0;

    if (window->autoAdvance) {
        if (window->autoAdvanceCounter++ >= window->autoAdvanceStep) {
            if (window->pageAdvanceTimer >= window->pageAdvanceDelay) {
                window->pageAdvanceDelay = result;
                window->pageAdvanceTimer = result;
                window->autoAdvanceCounter = result;
                result = 2;
            } else {
                window->pageAdvanceTimer++;
            }
        }
    } else if (window->pageAdvanceTimer >= window->pageAdvanceDelay) {
        window->pageAdvanceDelay = result;
        window->pageAdvanceTimer = result;
        if (gPadState[0] & 2) {
            window->fastPlayback = 1;
            result = 1;
        }
        if (gPadState[1] & 1)
            result = 1;
        window->widget.flags &= ~1;
    } else {
        window->fastPlayback = result;
        window->widget.flags |= 1;
        window->pageAdvanceTimer++;
    }
    if ((window->fastPlayback || result) && window->messageTable) {
        window->messageIndex++;
        if (window->fastPlayback) {
            MessageWindow_SetPlayback(window, MessageWindow_UpdateFastPlayback);
            st->characterDelay = 0;
            st->characterTimer = 0;
        } else {
            MessageWindow_SetPlayback(window, MessageWindow_UpdateNormalPlayback);
            window->inputDelay = 3;
            st->characterDelay = MessageWindow_GetCharacterDelay();
        }
        st->pc = MessageWindow_GetStringAtOffset(window, window->messageIndex);
        if (result != 2)
            Sound_PlaySong(0x107);
        TextWindow_BlankRect(window->x, window->y, window->width, window->height);
        TextState_SetBounds(st, window->x, window->y, window->width, window->height);
        TextState_Reset(&window->state);
        window->widget.flags |= 1;
    } else {
        Anim_Step(&window->widget, &window->resource);
    }
}

void MessageWindow_UpdateFastPlayback(struct MessageWindow *window)
{
    struct MessagePlaybackState *state = &window->state;
    if (window->fastPlayback)
        state->skip = 1;
    else
        state->skip = window->skipFlags;
    if (state->pc) {
        if (!state->stream && !*((u8 *)state->pc)) {
            int zero = 0;
            window->pageAdvanceTimer = zero;
            MessageWindow_SetPlayback(window, MessageWindow_UpdatePageAdvance);
            if (!window->autoAdvance && !window->pageAdvanceDelay)
                window->widget.flags &= ~1;
            goto done;
        } else {
            state->characterTimer = 0;
            state->instant = 1;
            if (TextState_StepPlayback(state) && state->complete)
                MessageWindow_SetPlayback(window, MessageWindow_ClearTextCompletion);
        }
    }
    if (window->callback != MessageWindow_ClearTextCompletion && !window->autoAdvance &&
        window->fastPlayback) {
        u16 mask = gPadState[0] & 2;
        if (!mask) {
            MessageWindow_SetPlayback(window, MessageWindow_UpdateNormalPlayback);
            window->fastPlayback = mask;
        }
    }
done:;
}

void MessageWindow_UpdateNormalPlayback(struct MessageWindow *window)
{
    struct MessagePlaybackState *state = &window->state;
    state->skip = window->skipFlags;
    if (state->pc) {
        if (!state->stream && !*((u8 *)state->pc)) {
            window->pageAdvanceTimer = 0;
            MessageWindow_SetPlayback(window, MessageWindow_UpdatePageAdvance);
            if (!window->autoAdvance && !window->pageAdvanceDelay)
                window->widget.flags &= ~1;
            return;
        }
        if (state->characterTimer++ >= state->characterDelay) {
            state->characterTimer = 0;
            state->instant = 0;
            if (TextState_StepPlayback(state)) {
                if (state->complete)
                    MessageWindow_SetPlayback(window, MessageWindow_ClearTextCompletion);
                else if (state->hold)
                    state->characterTimer = state->characterDelay;
            }
        }
    }
    if (window->callback != MessageWindow_ClearTextCompletion && !window->autoAdvance) {
        if (window->inputDelay == 0) {
            if (gPadState[1] & 1) {
                MessageWindow_SetPlayback(window, MessageWindow_UpdateFastPlayback);
                return;
            }
        } else
            window->inputDelay--;
        if (gPadState[0] & 2) {
            MessageWindow_SetPlayback(window, MessageWindow_UpdateFastPlayback);
            window->fastPlayback = 1;
        }
    }
}

void MessageWindow_ClearTextCompletion(struct MessageWindow *window)
{
    struct MessagePlaybackState *q = &window->state;
    q->complete = 0;
}

void MessageWindow_UpdateIdle(void)
{
}

void MessageWindow_UpdateImmediatePlayback(struct MessageWindow *window)
{
    struct MessagePlaybackState *state = &window->state;
    int i;
    void *close;
    void **callback;
    u32 *counter;
    state->skip = 1;
    i = 0;
    close = MessageWindow_ClearTextCompletion;
    callback = &window->callback;
    counter = &window->playbackCounter;
    do {
        if (state->pc) {
            state->characterTimer = 0;
            if (TextState_StepPlayback(state)) {
                *counter = 1;
                if (state->complete)
                    *callback = close;
                else
                    *callback = MessageWindow_UpdateIdle;
                break;
            }
        }
        i++;
    } while (i <= 3);
}

s32 MessageWindow_Update(struct MessageWindow *window)
{
    struct MessagePlaybackState *state = &window->state;
    if (state->frame) {
        window->pageAdvanceDelay = state->frame;
        state->frame = 0;
    }
    TextState_ClearSelectedChoices(state);
    if (window->queuedMessage >= 0)
        MessageWindow_ShowMessage(window->queuedMessage);
    if (window->callback)
        Callback_InvokeActor(window, window->callback);
    if (window->y + window->height <= 9) {
        window->widget.position.x = 0;
        window->widget.position.y = -102;
        window->widget.position.z = 0;
        window->portrait.position.x = 4;
        window->portrait.position.y = 0x40;
    } else {
        window->widget.position.x = 0;
        window->widget.position.y = 0;
        window->widget.position.z = 0;
        window->portrait.position.x = 4;
        window->portrait.position.y = 0x40;
    }
    if (window->callback != MessageWindow_ClearTextCompletion) {
        MessageWindow_ApplyPalette(window);
        if (window->previousSpeaker != state->speaker)
            MessageWindow_UpdateSpeakerPortrait(window);
    } else if (window->previousSpeaker != -1) {
        window->portrait.flags |= 1;
        window->previousSpeaker = -1;
    }
    TextState_RunSelectedProcedures(state);
    return 0;
}

int MessageWindow_OnDestroy(struct MessageWindow *window)
{
    Anim_Remove(&window->widget);
    Sprite_Remove(&window->portrait);
    return (u32)(gMessageWindow = 0);
}
