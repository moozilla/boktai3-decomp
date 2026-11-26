#include "ui/dialogue/message_window.h"
#include "player/cues/player_cue_sequence.h"
#include "ui/widgets/scene_input.h"

void TextWindow_InitializeGraphics(void);
void TextWindow_FillTileGraphics(void);
void TextState_Init(void *, s32, s32, s32, s32);

// confidence=low: opens a message window from script keywords (bounds, palette, text source, value list) and starts its script, sprite and animation setup.
int sub_0803198C(void *arg0, void *arg1)
{
    struct MessageWindow *p = arg0;
    struct MessagePlaybackState *st;
    u32 buf[2];
    struct SceneInputResource *proc;
    s32 palette;
    u32 bufferMask;
    void **fnSlot;

    gMessageWindow = (void *)p;
    st = &p->state;
    TextWindow_InitializeGraphics();
    TextWindow_FillTileGraphics();
    if (Script_SeekToKeyword('r')) {
        p->x = Script_GetValue();
        p->y = Script_GetValue();
        p->width = Script_GetValue();
        p->height = Script_GetValue();
    } else {
        p->x = 0;
        p->y = 0;
        p->width = 0;
        p->height = 0;
    }
    p->pageAdvanceTimer = 0;
    p->pageAdvanceDelay = Script_GetKeywordValue('l', 0);
    TextState_Init(st, p->x, p->y, p->width, p->height);
    p->displayMode = 0;
    if (Script_SeekToKeyword('s')) {
        void *pc = Script_GetTextSourcePc();
        p->messageTable = pc;
        if (!pc)
            return -1;
        st->pc = Text_LookupString(Script_ParseStringRef(pc));
        if (!st->pc)
            return -1;
    } else {
        p->messageTable = 0;
        st->pc = 0;
    }
    palette = Script_GetKeywordValue('c', 0x519C);
    if (MessageWindow_LoadPalette(p, palette) < 0)
        return -1;
    MessageWindow_ApplyPalette(p);
    st->spacing = 0;
    p->unknown185 = 0;
    p->messageIndex = Script_GetKeywordValue('i', 0);
    p->inputDelay = 3;
    p->fastPlayback = 0;
    p->autoAdvance = 0;
    p->autoAdvanceStep = 0;
    st->choiceCount = 0;
    if (Script_SeekToKeyword('p')) {
        fnSlot = &p->callback;
        while (Script_GetPc()) {
            st->choices[st->choiceCount] = Script_GetValue();
            st->choiceCount++;
        }
    } else {
        st->choiceCount = 0;
        fnSlot = &p->callback;
    }
    {
        struct WaitView {
            u16 value;
        };
        union {
            struct WaitView *wait;
            void *callback;
        } value;
        value.wait = (void *)&p->queuedMessage;
        value.wait->value = 0xFFFF;
        value.callback = MessageWindow_ClearTextCompletion;
        *fnSlot = value.callback;
        switch (p->displayMode) {
            case 0:
                proc = ScriptProc_Start(0xCB05, 0x530D);
                break;
            default:
                proc = ScriptProc_Start(0xCB05, 0x530D);
                break;
        }
        bufferMask = (u32)value.callback;
    }
    if (!proc)
        return -1;
    p->resource = *proc;
    ResourceHeader_Relocate(&p->resource, proc);
    bufferMask = 0xFFFF0000;
    buf[0] = 0;
    buf[1] &= bufferMask;
    Widget_InitLinked(&p->widget, &p->resource, 0x3D, 0x3071, 0, 1, 0x3C, buf);
    Anim_SetKeyframes(&p->widget, &p->resource, 0, 1, 0);
    AnimData_Load(p->portraitGraphics, 0x593A);
    p->portraitResource = ScriptProc_Start(0x922E, 0x4BBF);
    Sprite_Init(&p->portrait, p->portraitGraphics, 0x1051);
    p->portrait.priority = 0;
    Anim_SetSequence(&p->portraitSequence, p->portraitResource, 0, 0, 0);
    PlayerCue_StepSequence(&p->portrait, &p->portraitSequence);
    p->portrait.position.x = 4;
    p->portrait.position.y = 0x40;
    p->portrait.position.z = 0;
    return 0;
}
