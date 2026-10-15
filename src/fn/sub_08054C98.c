#include "display_track.h"
#define DisplayTrack_SetPriorityFromScript sub_08054C98
void DisplayTrack_SetPriorityFromScript(void)
{
    struct DisplayTrack *track = sub_08054B80();
    if (track && Script_SeekToKeyword('p'))
        track->sprite.priority = Script_GetValue();
}
