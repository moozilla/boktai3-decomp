#include "global.h"
#include "minigames/result_game/result_tutorial.h"
extern u8 gUnk_0824DB2D[];
void ResultMenu_EnqueueMarker(struct ResultMenu *menu);
void ResultGame_ScoreAlignment(struct ResultMenu *menu)
{
    int delta = menu->playerPosition - menu->targetPosition;
    int limit, value, width;
    if (delta < 0)
        delta = -delta;
    limit = menu->wideTimingWindow ? 160 : 96;
    width = gUnk_0824DB2D[menu->difficulty] << 5;
    value = menu->trackingScore;
    if (delta <= limit) {
        value += 5;
        if (value > 65535)
            value = 65535;
        menu->trackingScore = value;
        menu->preciseCount++;
        if (menu->lastTimingGrade) {
            Sound_StopSong(0x3ee);
            Sound_PlaySong(0x3ed);
        }
        menu->lastTimingGrade = 0;
    } else if (delta <= width) {
        value += 3;
        if (value > 65535)
            value = 65535;
        menu->trackingScore = value;
        menu->closeCount++;
        if (delta > 320)
            Sound_StopSong(0x3ed);
        if (menu->lastTimingGrade != 1)
            Sound_StopSong(0x3ee);
        menu->lastTimingGrade = 1;
    } else {
        if (value > 65535)
            value = 65535;
        menu->trackingScore = value;
        menu->missedCount++;
        if (menu->lastTimingGrade != 2) {
            Sound_StopSong(0x3ed);
            Sound_PlaySong(0x3ee);
        }
        menu->lastTimingGrade = 2;
    }
    ResultMenu_EnqueueMarker(menu);
    menu->trackingSamples++;
}
