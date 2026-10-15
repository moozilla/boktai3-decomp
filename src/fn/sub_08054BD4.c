#include "display_track.h"
#define DisplayTrack_SetTargetFromScript sub_08054BD4
void DisplayTrack_SetTargetFromScript(void)
{
    struct DisplayTrack *track = sub_08054B80();
    if (track && Script_SeekToKeyword('p')) {
        track->target.x = Script_GetValue();
        track->target.y = Script_GetValue();
        track->target.z = Script_GetValue();
        if (Script_SeekToKeyword('f')) {
            track->moveDuration = Script_GetValue();
            if (track->moveDuration) {
                track->flags |= 1;
                track->origin = track->sprite.position;
                track->moveFrame = 0;
            }
        }
    }
}
