#include "minigames/result_game_tutorial/result_tutorial.h"
extern u8 gUnk_0824DB2D[];
void ResultMenu_EnqueueMarker(void *);
void ResultGame_ScorePress(struct ResultMenu *p)
{
    int delta = p->playerPosition - p->targetPosition;
    int limit, value, width;
    if (delta < 0)
        delta = -delta;
    limit = p->wideTimingWindow ? 160 : 96;
    width = gUnk_0824DB2D[p->difficulty] << 5;
    value = p->pressScoreTotal;
    if (delta <= limit) {
        value += 5;
        if (value > 65535)
            value = 65535;
        p->pressScoreTotal = value;
        ++p->preciseCount;
        p->lastTimingGrade = 0;
        Sound_PlaySong(0x394);
    } else if (delta <= width) {
        value += 3;
        if (value > 65535)
            value = 65535;
        p->pressScoreTotal = value;
        ++p->closeCount;
        p->lastTimingGrade = 1;
        Sound_PlaySong(0x395);
    } else {
        if (value > 65535)
            value = 65535;
        p->pressScoreTotal = value;
        ++p->missedCount;
        p->lastTimingGrade = 2;
        Sound_PlaySong(0x396);
    }
    ResultMenu_EnqueueMarker(p);
    ++p->pressSamples;
}
