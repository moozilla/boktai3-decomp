#include "result_tutorial.h"

extern u8 gUnk_0824DB8A[], gUnk_0824DB7A[];
void ResultGame_HidePressMarker(void *);
void ResultGame_ShowTargetMarker(void *);
void ResultGame_SetTrackingVisibility(void *);
void ResultGame_HideTargetMarker(void *);
void ResultGame_UpdatePlayerMarker(void *);
void ResultGame_ScoreAlignment(void *);
void ResultGame_SetUpdateCallback(struct ResultMenu *, void (*)(struct ResultMenu *));
void ResultGame_TimedPress(struct ResultMenu *);

void ResultGame_TrackTarget(struct ResultMenu *p)
{
    int speed = gUnk_0824DB8A[p->difficulty];

    switch (p->phase) {
        case 0:
            p->playerPosition = 0;
            p->playerVelocity = 0;
            p->accelerationDelay = 0;
            p->targetPosition = 0;
            p->targetMinimum = gUnk_0824DB7A[p->difficulty * 2] << 5;
            p->targetMaximum = (gUnk_0824DB7A + 1)[p->difficulty * 2] << 5;
            ++p->phase;
            p->timer = 0;
            p->elapsed = 0;
            p->gaugeBrightness = 32;
            p->lastTimingGrade = 255;
            ResultGame_HidePressMarker(p);
            ResultGame_ShowTargetMarker(p);
            ResultGame_SetTrackingVisibility(p);
            Sound_PlaySong(0x4b1);
            break;

        case 1:
            p->playerPosition += 176;
            p->targetPosition = p->playerPosition;
            if (p->targetPosition >= p->targetMaximum) {
                p->phase = 4;
                p->elapsed = 0;
            }
            break;

        case 2:
            p->targetPosition += speed;
            ResultGame_UpdatePlayerMarker(p);
            if ((p->elapsed & 15) == 0)
                ResultGame_ScoreAlignment(p);
            if (p->targetPosition >= p->targetMaximum) {
                ++p->phase;
                p->timer = 30;
            }
            break;

        case 3:
            ResultGame_UpdatePlayerMarker(p);
            if ((p->elapsed & 15) == 0)
                ResultGame_ScoreAlignment(p);
            if (p->timer <= 0)
                ++p->phase;
            break;

        case 4:
            p->targetPosition -= speed;
            ResultGame_UpdatePlayerMarker(p);
            if ((p->elapsed & 15) == 0)
                ResultGame_ScoreAlignment(p);
            if (p->targetPosition <= p->targetMinimum) {
                ++p->phase;
                p->timer = 10;
            }
            break;

        case 5:
            ResultGame_UpdatePlayerMarker(p);
            if ((p->elapsed & 15) == 0)
                ResultGame_ScoreAlignment(p);
            if (p->timer <= 0)
                p->phase = 2;
            break;

        case 6:
            p->playerPosition -= 176;
            if (p->playerPosition <= 0) {
                p->playerPosition = 0;
                ResultGame_SetUpdateCallback(p, ResultGame_TimedPress);
                if (p->finishState == 1)
                    p->finishState = 2;
            }
            break;
    }

    p->gaugeBrightness = 32 - Div(p->elapsed << 5, 1200);
    --p->timer;
    if (++p->elapsed > 1200)
        p->elapsed = 1200;
    if (p->phase != 6 && (p->finishState == 1 || p->elapsed > 1199)) {
        Sound_StopSong(0x3ed);
        Sound_StopSong(0x3ee);
        p->phase = 6;
        p->gaugeBrightness = 0;
        ResultGame_HideTargetMarker(p);
    }
}
