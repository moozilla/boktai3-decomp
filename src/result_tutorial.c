#include "result_tutorial.h"
#include "script_utility.h"

void Blend_SetAlpha(int, int, int);
void MessageWindow_ShowMessage(int);
void ResultGame_ShowTargetMarker(void *);
void ResultGame_HideTargetMarker(void *);
void ResultGame_SetPressVisibility(void *);
void ResultGame_ScoreAlignment(void *);
void ResultGame_ScorePress(void *);
void ResultGame_ShowPressMarker(void *);
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void Sound_PauseChannel10(int);
void ScrollingDialogue_RequestHide(void);
int ResultMenu_ConfigureLayers(int, int);
void ResultMenu_InitializeLayer(void);
void MessageWindow_SetMessageTable(void *);
void MessageWindow_SetBounds(int, int, int, int);
void MessageWindow_SetPalette(int);
void MessageWindow_SetDisplayMode(int);
void sub_0801BD44(void *);
void ResultMenu_SetPhase(void *, void *);
void sub_0801C24C(void *);
void *Script_GetTextSourcePc(void);
void ResultMenu_CreateGraphics(void *);
void Sound_PlayTrackedSong(int);
void ResultMenu_OnTaskExit(void *);
int ResultTutorial_FadeIn(struct ResultMenu *tutorial)
{
    int result;
    if (tutorial->alpha != 16) {
        int fade = tutorial->timer >> 1;
        Blend_SetAlpha(0, 8 - fade, fade + 8);
        tutorial->alpha = fade + 8;
        ++tutorial->timer;
        result = 0;
    } else
        result = 1;
    return result;
}

int ResultTutorial_FadeOut(struct ResultMenu *tutorial)
{
    int result;
    if (tutorial->alpha != 8) {
        int fade = tutorial->timer >> 1;
        int alpha = 16 - fade;
        Blend_SetAlpha(0, fade, alpha);
        tutorial->alpha = alpha;
        ++tutorial->timer;
        result = 0;
    } else
        result = 1;
    return result;
}

int ResultTutorial_ShowPrompt(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            MessageWindow_ShowMessage(tutorial->prompt);
            ++tutorial->phase;
            tutorial->timer = 0;
        case 1:
            if (ResultTutorial_FadeIn(tutorial)) {
                {
                    struct SceneInputWidget *promptWidget = &tutorial->widgets[14];
                    promptWidget->flags &= ~1;
                }
                return 1;
            }
            break;
    }
    return 0;
}

int ResultTutorial_HidePrompt(struct ResultMenu *tutorial)
{
    u8 *state = &tutorial->phase;
    switch (*state) {
        case 0: {
            int one = 1;
            *state = one;
            tutorial->timer = 0;
            {
                struct SceneInputWidget *promptWidget = &tutorial->widgets[14];
                promptWidget->flags |= one;
            }
        }
        case 1:
            if (ResultTutorial_FadeOut(tutorial))
                return 1;
            break;
    }
    return 0;
}

int ResultTutorial_SlideMarkerIn(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            ResultGame_ShowTargetMarker(tutorial);
            tutorial->playerPosition = 0;
            tutorial->targetPosition = 0;
            ++tutorial->phase;
            tutorial->timer = 0;
            break;
        case 1:
            tutorial->playerPosition += 176;
            tutorial->targetPosition = tutorial->playerPosition;
            if (tutorial->playerPosition > 3584) {
                tutorial->playerPosition = 3584;
                return 1;
            }
            break;
    }
    return 0;
}

int ResultTutorial_BlinkPlayer(struct ResultMenu *tutorial)
{
    int state = tutorial->phase;
    switch (state) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
            break;
        case 1: {
            int frame = tutorial->timer & 31;
            int visible;
            if (frame == 0)
                Sound_PlaySong(0x2b0);
            visible = 1;
            if (frame <= 19)
                goto show;
            if (frame <= 29)
                visible = 0;
            if (visible)
            show: {
                struct SceneInputWidget *playerWidget = &tutorial->widgets[4];
                playerWidget->flags &= ~1;
            }
                else
                {
                    struct SceneInputWidget *playerWidget = &tutorial->widgets[4];
                    playerWidget->flags |= state;
                }
            if ((tutorial->timer >> 5) > 2)
                return 1;
            ++tutorial->timer;
            break;
        }
    }
    return 0;
}

int ResultTutorial_BlinkTarget(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
            break;
        case 1: {
            int frame = tutorial->timer & 31;
            int visible;
            if (frame == 0)
                Sound_PlaySong(0x2b0);
            visible = 1;
            if (frame <= 19)
                goto show;
            if (frame <= 29)
                visible = 0;
            if (visible)
            show:
                ResultGame_ShowTargetMarker(tutorial);
            else
                ResultGame_HideTargetMarker(tutorial);
            if ((tutorial->timer >> 5) > 2)
                return 1;
            ++tutorial->timer;
            break;
        }
    }
    return 0;
}

extern int gUnk_0824DB38[];

extern u8 gUnk_0824DB64[], gUnk_0824DB6F[];

int ResultTutorial_MovePlayer(struct ResultMenu *tutorial, int input)
{
    int *velocity;
    if (input) {
        int *initial = &tutorial->playerVelocity;
        *initial = gUnk_0824DB38[4];
        velocity = initial;
    } else {
        int *initial = &tutorial->playerVelocity;
        int value;
        value = *initial;
        velocity = initial;
        if (value > 0) {
            *velocity = input;
            tutorial->accelerationDelay = input;
        } else if (tutorial->accelerationDelay < gUnk_0824DB64[4]) {
            *velocity = input;
            ++tutorial->accelerationDelay;
        } else {
            *velocity = value - gUnk_0824DB6F[4];
            if (*velocity < -22)
                *velocity = -22;
        }
    }
    tutorial->playerPosition += *velocity;
}

u32 ResultTutorial_DemonstrateMovement(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            tutorial->phase++;
            tutorial->timer = 0;
            break;
        case 1:
        case 3:
        case 5:
            tutorial->timer++;
            if ((tutorial->timer & 3) == 0) {
                ResultTutorial_MovePlayer(tutorial, 1);
            } else {
                ResultTutorial_MovePlayer(tutorial, 0);
            }
            if (tutorial->playerPosition > 0x1000) {
                tutorial->phase++;
            }
            break;
        case 2:
        case 4:
        case 6:
            ResultTutorial_MovePlayer(tutorial, 0);
            if (tutorial->playerPosition <= 0xBFF) {
                tutorial->phase++;
                if (tutorial->phase > 5)
                    return 1;
            }
            break;
        default:
            break;
    }
    return 0;
}

int ResultTutorial_RaiseMarker(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
        case 1:
            if ((++tutorial->timer & 3) == 0)
                ResultTutorial_MovePlayer(tutorial, 1);
            else
                ResultTutorial_MovePlayer(tutorial, 0);
            if (tutorial->playerPosition > 4096)
                return 1;
            break;
    }
    return 0;
}

int ResultTutorial_LowerMarker(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
            break;
        case 1:
            ResultTutorial_MovePlayer(tutorial, 0);
            if (tutorial->playerPosition <= 3071)
                return 1;
            break;
    }
    return 0;
}

extern u8 gUnk_0824DB8A[];

int ResultTutorial_DemonstrateTracking(struct ResultMenu *tutorial)
{
    int speed = gUnk_0824DB8A[tutorial->difficulty];
    switch (tutorial->phase) {
        case 0: {
            int next = tutorial->phase + 1;
            int zero;
            zero = 0;
            tutorial->phase = next;
            tutorial->elapsed = zero;
            tutorial->timer = zero;
            break;
        }
        case 1:
        case 5:
        case 9:
            tutorial->targetPosition += speed;
            if (tutorial->targetPosition > 4096) {
                int next = tutorial->phase + 1;
                int zero;
                zero = 0;
                tutorial->phase = next;
                tutorial->timer = zero;
            }
            break;
        case 2:
        case 6:
        case 10:
            if (++tutorial->timer > 30)
                ++tutorial->phase;
            break;
        case 3:
        case 7:
        case 11:
            tutorial->targetPosition -= speed;
            if (tutorial->targetPosition <= 3071) {
                int next = tutorial->phase + 1;
                int zero;
                zero = 0;
                tutorial->phase = next;
                tutorial->timer = zero;
            }
            break;
        case 4:
        case 8:
        case 12:
            if (++tutorial->timer > 10) {
                if (++tutorial->phase > 11)
                    return 1;
            }
            break;
    }
    if (tutorial->playerPosition < tutorial->targetPosition - 96)
        ResultTutorial_MovePlayer(tutorial, 1);
    else
        ResultTutorial_MovePlayer(tutorial, 0);
    if ((tutorial->elapsed & 15) == 0)
        ResultGame_ScoreAlignment(tutorial);
    ++tutorial->elapsed;
    return 0;
}

int ResultTutorial_SlideMarkerOut(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
            ResultGame_HideTargetMarker(tutorial);
            break;
        case 1:
            tutorial->playerPosition -= 176;
            if (tutorial->playerPosition <= 0) {
                tutorial->playerPosition = 0;
                return 1;
            }
            break;
    }
    return 0;
}

int ResultTutorial_InitializePressDemo(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            ResultGame_ShowTargetMarker(tutorial);
            ResultGame_SetPressVisibility(tutorial);
            tutorial->pressState = 255;
            tutorial->pressed = 0;
            tutorial->playerPosition = 0;
            tutorial->targetPosition = 1536;
            ++tutorial->phase;
            tutorial->timer = 0;
            break;
        case 1:
            tutorial->playerPosition += 1024;
            if (tutorial->playerPosition > 3584) {
                tutorial->playerPosition = 3584;
                return 1;
            }
            break;
    }
    return 0;
}

int ResultTutorial_BlinkPressMarker(struct ResultMenu *tutorial)
{
    int state = tutorial->phase;
    switch (state) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
            break;
        case 1: {
            int frame = tutorial->timer & 31;
            int visible;
            if (frame == 0)
                Sound_PlaySong(0x2b0);
            visible = 1;
            if (frame <= 19)
                goto show;
            if (frame <= 29)
                visible = 0;
            if (visible)
            show: {
                struct SceneInputWidget *playerWidget = &tutorial->widgets[4];
                playerWidget->flags &= ~1;
            }
                else
                {
                    struct SceneInputWidget *playerWidget = &tutorial->widgets[4];
                    playerWidget->flags |= state;
                }
            if ((tutorial->timer >> 5) > 2)
                return 1;
            ++tutorial->timer;
            break;
        }
    }
    return 0;
}

void ResultTutorial_Press(struct ResultMenu *tutorial, int pressed)
{
    int state = (s8)tutorial->pressState;
    if (state == 0 && pressed) {
        tutorial->pressState = 1;
        tutorial->pressed = 1;
        ResultGame_ScorePress(tutorial);
        {
            u32 v = tutorial->lastPressScore;
            tutorial->pressScore = v;
        }
        ResultGame_ShowPressMarker(tutorial);
    }
}

extern int gUnk_0824DB94[];

extern u8 gUnk_0824DB2D[];

int ResultTutorial_DemonstrateEarlyPress(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            tutorial->phase = 1;
            tutorial->pressState = 255;
            tutorial->pressed = 0;
            tutorial->timer = 0;
            break;
        case 1:
            tutorial->playerPosition += 1024;
            tutorial->pressState = 255;
            if (tutorial->playerPosition > 5631) {
                tutorial->pressState = 0;
                tutorial->playerPosition = 5632;
                ++tutorial->phase;
            }
            break;
        case 2: {
            int width;
            tutorial->playerPosition -= gUnk_0824DB94[4];
            width = gUnk_0824DB2D[tutorial->difficulty] << 5;
            if (tutorial->playerPosition - tutorial->targetPosition < width + 512)
                ResultTutorial_Press(tutorial, 1);
            if (tutorial->playerPosition <= 0) {
                tutorial->playerPosition = 0;
                return 1;
            }
            break;
        }
    }
    return 0;
}

int ResultTutorial_DemonstrateLatePress(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            tutorial->phase = 1;
            tutorial->pressState = 255;
            tutorial->pressed = 0;
            tutorial->timer = 0;
            break;
        case 1:
            tutorial->playerPosition += 1024;
            tutorial->pressState = 255;
            if (tutorial->playerPosition > 5631) {
                tutorial->playerPosition = 5632;
                tutorial->pressState = 0;
                ++tutorial->phase;
            }
            break;
        case 2: {
            int width;
            tutorial->playerPosition -= gUnk_0824DB94[4];
            width = gUnk_0824DB2D[tutorial->difficulty] << 5;
            if (tutorial->playerPosition - tutorial->targetPosition < width - 256)
                ResultTutorial_Press(tutorial, 1);
            if (tutorial->playerPosition <= 0) {
                tutorial->playerPosition = 0;
                return 1;
            }
            break;
        }
    }
    return 0;
}

int ResultTutorial_DemonstrateAccuratePress(struct ResultMenu *tutorial)
{
    int state = tutorial->phase;
    int *alias;
    switch (state) {
        case 0:
            tutorial->phase = 1;
            tutorial->pressState = 255;
            tutorial->pressed = state;
            tutorial->timer = state;
            break;

        case 1:
            tutorial->playerPosition += 1024;
            tutorial->pressState = 255;
            if ((tutorial->playerPosition) > 5631) {
                tutorial->playerPosition = 5632;
                tutorial->pressState = 0;
                ++tutorial->phase;
            }
            break;

        case 2:
            alias = &tutorial->playerPosition;
            *alias -= gUnk_0824DB94[4];
            state = tutorial->playerPosition;
            if ((state - (tutorial->targetPosition)) <= 95) {
                ResultTutorial_Press(tutorial, 1);
            }
            if ((tutorial->playerPosition) <= 0) {
                tutorial->playerPosition = 0;
                return 1;
            }
            break;
    }

    return 0;
}

int ResultTutorial_BlinkScore(struct ResultMenu *tutorial)
{
    int state = tutorial->phase;
    switch (state) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
            break;
        case 1: {
            u32 *flags = &tutorial->markers[0].flags;
            int frame = tutorial->timer & 31;
            int visible;
            if (frame == 0)
                Sound_PlaySong(0x2b0);
            visible = 1;
            if (frame <= 19)
                goto show;
            if (frame <= 29)
                visible = 0;
            if (visible)
            show:
                *flags &= ~1;
            else
                *flags |= state;
            if ((tutorial->timer >> 5) > 2) {
                *flags |= 1;
                return 1;
            }
            ++tutorial->timer;
            break;
        }
    }
    return 0;
}

int ResultTutorial_BlinkTimer(struct ResultMenu *tutorial)
{
    int state = tutorial->phase;
    switch (state) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
            break;
        case 1: {
            int frame = tutorial->timer & 31;
            int visible;
            if (frame == 0)
                Sound_PlaySong(0x2b0);
            visible = 1;
            if (frame <= 19)
                goto show;
            if (frame <= 29)
                visible = 0;
            if (visible)
            show:
                tutorial->hideTimer = 0;
            else
                tutorial->hideTimer = state;
            if ((tutorial->timer >> 5) > 2)
                return 1;
            ++tutorial->timer;
            break;
        }
    }
    return 0;
}

int ResultTutorial_BlinkLevel(struct ResultMenu *tutorial)
{
    int state = tutorial->phase;
    switch (state) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
            break;
        case 1: {
            int frame = tutorial->timer & 31;
            int visible;
            if (frame == 0)
                Sound_PlaySong(0x2b0);
            visible = 1;
            if (frame <= 19)
                goto show;
            if (frame <= 29)
                visible = 0;
            if (visible)
            show:
                tutorial->hideLevel = 0;
            else
                tutorial->hideLevel = state;
            if ((tutorial->timer >> 5) > 2)
                return 1;
            ++tutorial->timer;
            break;
        }
    }
    return 0;
}

int ResultTutorial_AnimateLevel(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            tutorial->phase = 1;
            tutorial->timer = 0;
            break;
        case 1:
            tutorial->level = (tutorial->timer >> 4) + 3;
            if (++tutorial->timer > 94) {
                ++tutorial->phase;
                tutorial->timer = 0;
            }
            break;
        case 2:
            tutorial->level = 8 - (tutorial->timer >> 4);
            if (++tutorial->timer > 78)
                return 1;
            break;
    }
    return 0;
}

int ResultTutorial_Close(struct ResultMenu *tutorial)
{
    int value;
    struct ScriptUtilityArguments args;
    switch (tutorial->phase) {
        case 0:
            ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xffff, 0);
            Sound_PauseChannel10(5);
            ++tutorial->phase;
            tutorial->timer = 0;
            break;
        case 1:
            if (++tutorial->timer > 31) {
                args.count = 0;
                args.values = &value;
                if (tutorial->completionScript)
                    Script_Call(tutorial->completionScript, &args);
                {
                    volatile u32 *flags = &gEnginePauseFlags;
                    u32 mask;
                    mask = ~4;
                    *flags &= mask;
                }
                ScrollingDialogue_RequestHide();
                Task_Kill(tutorial);
                return 1;
            }
            break;
    }
    return 0;
}

void ResultTutorial_Open(struct ResultMenu *tutorial)
{
    switch (tutorial->phase) {
        case 0:
            tutorial->layer = ResultMenu_ConfigureLayers(14, 1);
            ResultMenu_InitializeLayer();
            MessageWindow_SetMessageTable(tutorial->messageTable);
            MessageWindow_SetBounds(1, 1, 30, 6);
            MessageWindow_SetPalette(0x81b);
            MessageWindow_SetDisplayMode(1);
            sub_0801BD44(tutorial);
            MessageWindow_ShowMessage(tutorial->prompt);
            ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xffff, 0);
            ++tutorial->phase;
            tutorial->timer = 0;
            break;
        case 1:
            if (++tutorial->timer > 31) {
                {
                    struct SceneInputWidget *promptWidget = &tutorial->widgets[14];
                    promptWidget->flags &= ~1;
                }
                tutorial->alpha = 16;
                ResultMenu_SetPhase(tutorial, ResultTutorial_UpdateSequence);
            }
            break;
    }
}

extern struct ResultTutorialStep gUnk_0824DBC0[];

void ResultTutorial_UpdateSequence(struct ResultMenu *tutorial)
{
    int index = tutorial->step >> 2;
    switch (tutorial->step & 3) {
        case 0:
            if (gUnk_0824DBC0[index].prompt >= 0) {
                tutorial->prompt = gUnk_0824DBC0[index].prompt;
                if (ResultTutorial_ShowPrompt(tutorial)) {
                    ++tutorial->step;
                    tutorial->phase = 0;
                }
                break;
            }
            ++tutorial->step;
            tutorial->phase = 0;
        case 1:
            if (gUnk_0824DBC0[index].wait) {
                if (gPadState[1] & 1) {
                    ++tutorial->step;
                    Sound_PlaySong(0x107);
                }
                break;
            }
            ++tutorial->step;
            tutorial->phase = 0;
        case 2:
            if (gUnk_0824DBC0[index].hide) {
                if (ResultTutorial_HidePrompt(tutorial)) {
                    int next = tutorial->step + 1;
                    int zero;
                    zero = 0;
                    tutorial->step = next;
                    tutorial->phase = zero;
                }
                break;
            }
            ++tutorial->step;
            tutorial->phase = 0;
        case 3:
            if (gUnk_0824DBC0[index].run) {
                if (sub_08249240(tutorial, &gUnk_0824DBC0[index].run, gUnk_0824DBC0[index].run)) {
                    int next = tutorial->step + 1;
                    int zero;
                    zero = 0;
                    tutorial->step = next;
                    tutorial->phase = zero;
                    Sound_StopSong(0x3ed);
                    Sound_StopSong(0x3ee);
                }
                --tutorial->countdown;
            } else {
                ++tutorial->step;
                tutorial->phase = 0;
            }
            break;
    }
    sub_0801C24C(tutorial);
}

u32 ResultTutorial_Finish(struct ResultMenu *tutorial)
{
    Callback_InvokeActor(tutorial, tutorial->update);
    sub_0801C24C(tutorial);
    return 0;
}

int ResultTutorial_ReadScript(struct ResultMenu *tutorial)
{
    int result;
    tutorial->completionScript = Script_GetKeywordValue(0x65, 0);
    if (!Script_SeekToKeyword(0x6b) || !(tutorial->messageTable = Script_GetTextSourcePc()))
        result = -1;
    else
        result = 0;
    return result;
}

int ResultTutorial_Initialize(struct ResultMenu *tutorial, void *owner, void *unused)
{
    struct SceneInputResource *r;
    int four;
    u8 *difficulty;
    int zero;
    tutorial->owner = owner;
    r = ScriptProc_Start(0xcb05, 0x530d);
    if (!r) {
        return -1;
    }
    tutorial->primaryResource = *r;
    ResourceHeader_Relocate(&tutorial->primaryResource, r);
    r = ScriptProc_Start(0xcb05, 0x5d04);
    if (!r) {
        return -1;
    }
    tutorial->secondaryResource = *r;
    ResourceHeader_Relocate(&tutorial->secondaryResource, r);
    if (ResultTutorial_ReadScript(tutorial) < 0) {
        return -1;
    }
    difficulty = &tutorial->difficulty;
    zero = 0;
    four = 4;
    *difficulty = four;
    tutorial->step = zero;
    tutorial->prompt = zero;
    tutorial->level = 3;
    ResultMenu_CreateGraphics(tutorial);
    ResultMenu_SetPhase(tutorial, ResultTutorial_Open);
    Sound_PlayTrackedSong(0xaa);
    gEnginePauseFlags |= four;
    return 0;
}

void *ResultTutorial_CreateTask(void *a, void *b)
{
    void *obj;
    obj = Task_Create(11, 0x7F4);
    if (obj != 0) {
        Task_SetCallbacks(obj, (void (*)(void *))ResultTutorial_Finish, ResultMenu_OnTaskExit);
        if (ResultTutorial_Initialize(obj, a, b) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
