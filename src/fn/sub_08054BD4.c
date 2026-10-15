#include "display_track.h"
void DisplayTrack_SetTargetFromScript(void)
{
    struct DisplayTrack *track = DisplayTrack_FindFromScript();
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
