#include "minigames/result_game_tutorial/result_tutorial.h"
#include "engine/shared/player.h"
extern int gUnk_0824DB94[];
void ResultGame_ShowTargetMarker(void *);
void ResultGame_SetPressVisibility(void *);
void PlayerCue_BeginModeSeven(void *, int);
void ResultMenu_SendCommand(void *, int, int, void *);
void ResultGame_HandlePress(void *);
void ResultGame_ScorePress(struct ResultMenu *);
void ResultGame_HidePressMarker(void *);
void ResultGame_HideTargetMarker(void *);
void ResultGame_SetUpdateCallback(struct ResultMenu *, void (*)(struct ResultMenu *));
void ResultGame_TrackTarget(struct ResultMenu *);
enum ResultPressState { RESULT_PRESS_READY = 0, RESULT_PRESS_DISABLED = 255 };

void ResultGame_TimedPress(struct ResultMenu *p)
{
    switch (p->phase) {
        case 0:
            p->playerPosition = 0;
            p->gaugeBrightness = 0;
            p->targetPosition = 1536;
            ResultGame_ShowTargetMarker(p);
            ResultGame_SetPressVisibility(p);
            p->pressState = RESULT_PRESS_DISABLED;
            p->pressed = 0;
            ++p->phase;
            p->lastTimingGrade = 255;
            PlayerCue_BeginModeSeven(gPlayers[0], 1);
            ResultMenu_SendCommand(p, 0x35, 0, 0);
            Sound_PlaySong(0x4b2);
            break;
        case 1:
            ++p->phase;
            p->timer = 60;
            p->elapsed = 0;
            p->pressState = RESULT_PRESS_DISABLED;
            p->pressed = 0;
            break;
        case 2:
            if (p->timer <= 0)
                ++p->phase;
            break;
        case 3:
            p->playerPosition += 1024;
            p->pressState = RESULT_PRESS_DISABLED;
            if (p->playerPosition > 5631) {
                p->playerPosition = 5632;
                ++p->phase;
                p->pressState = RESULT_PRESS_READY;
            }
            break;
        case 4:
            p->playerPosition -= gUnk_0824DB94[p->level];
            ResultGame_HandlePress(p);
            if (p->playerPosition <= 0) {
                if ((s8)p->pressState == RESULT_PRESS_READY) {
                    ResultGame_ScorePress(p);
                    ResultGame_HidePressMarker(p);
                    ResultMenu_SendCommand(p, 0x34, 0, 0);
                    p->pressed = 1;
                }
                p->pressState = RESULT_PRESS_DISABLED;
                p->playerPosition = 0;
                p->phase = 3;
                if (p->finishState == 1 || p->elapsed > 899) {
                    ResultGame_SetUpdateCallback(p, ResultGame_TrackTarget);
                    ResultGame_HideTargetMarker(p);
                    ResultGame_HidePressMarker(p);
                    if (p->finishState == 1) {
                        p->finishState = 2;
                    } else {
                        ResultMenu_SendCommand(p, 0x36, 0, 0);
                        PlayerCue_BeginModeSeven(gPlayers[0], 0);
                    }
                }
            }
            break;
    }
    p->gaugeBrightness = Div(p->elapsed << 5, 900);
    ++p->elapsed;
    --p->timer;
    if (p->elapsed > 900)
        p->elapsed = 900;
}
