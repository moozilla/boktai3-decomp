#include "ui/dialogue/message_window.h"
#include "player/cutscene/player_cue_sequence.h"
#include "ui/widgets/scene_input.h"

void TextWindow_InitializeGraphics(void);
void TextWindow_FillTileGraphics(void);
void TextState_Init(void *, s32, s32, s32, s32);

// confidence=low: opens a message window from script keywords (bounds, palette, text source, value list) and starts its script, sprite and animation setup.
struct QueuedMessageView {
    u16 value;
};

int sub_0803198C(void *arg0, void *arg1)
{
    struct MessageWindow *window = arg0;
    struct MessagePlaybackState *state;
    struct AnimSpriteResource *resource;
    void *field;
    struct ContactPosition position;

    gMessageWindow = (void *)window;
    state = &window->state;
    TextWindow_InitializeGraphics();
    TextWindow_FillTileGraphics();
    if (Script_SeekToKeyword('r')) {
        window->x = Script_GetValue();
        window->y = Script_GetValue();
        window->width = Script_GetValue();
        window->height = Script_GetValue();
    } else {
        window->x = 0;
        window->y = 0;
        window->width = 0;
        window->height = 0;
    }
    window->pageAdvanceTimer = 0;
    window->pageAdvanceDelay = Script_GetKeywordValue('l', 0);
    TextState_Init(state, window->x, window->y, window->width, window->height);
    window->displayMode = 0;
    if (Script_SeekToKeyword('s')) {
        if (!(window->messageTable = Script_GetTextSourcePc()))
            return -1;
        if (!(state->pc = Text_LookupString(Script_ParseStringRef(window->messageTable))))
            return -1;
    } else {
        window->messageTable = 0;
        state->pc = 0;
    }
    if (MessageWindow_LoadPalette(window, Script_GetKeywordValue('c', 0x519C)) < 0)
        return -1;
    MessageWindow_ApplyPalette(window);
    state->spacing = 0;
    window->unknown185 = 0;
    window->messageIndex = Script_GetKeywordValue('i', 0);
    window->inputDelay = 3;
    window->fastPlayback = 0;
    window->autoAdvance = 0;
    window->autoAdvanceStep = 0;
    state->choiceCount = 0;
    if (Script_SeekToKeyword('p')) {
        while (Script_GetPc()) {
            state->choices[state->choiceCount] = Script_GetValue();
            state->choiceCount++;
        }
    } else {
        state->choiceCount = 0;
    }
    field = &window->queuedMessage;
    ((struct QueuedMessageView *)field)->value = 0xFFFF;
    field = MessageWindow_ClearTextCompletion;
    MessageWindow_SetPlayback(window, field);
    switch (window->displayMode) {
        case 0:
            resource = ScriptProc_Start(0xCB05, 0x530D);
            break;
        default:
            resource = ScriptProc_Start(0xCB05, 0x530D);
            break;
    }
    if (!resource)
        return -1;
    window->resource = *resource;
    ResourceHeader_Relocate(&window->resource, resource);
    position.x = 0;
    position.y = 0;
    position.z = 0;
    AnimSprite_InitLinked(&window->animSprite, &window->resource, 0x3D, 0x3071, 0, 1, 0x3C,
                          &position);
    Anim_SetKeyframes(&window->animSprite, &window->resource, 0, 1, 0);
    AnimData_Load(window->portraitGraphics, 0x593A);
    window->portraitResource = ScriptProc_Start(0x922E, 0x4BBF);
    Sprite_Init(&window->portrait, window->portraitGraphics, 0x1051);
    window->portrait.priority = 0;
    Anim_SetSequence(&window->portraitSequence, window->portraitResource, 0, 0, 0);
    PlayerCue_StepSequence(&window->portrait, &window->portraitSequence);
    window->portrait.position.x = 4;
    window->portrait.position.y = 0x40;
    window->portrait.position.z = 0;
    return 0;
}
