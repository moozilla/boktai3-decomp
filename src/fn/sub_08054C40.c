#include "display_track.h"
#define DisplayTrack_SetFrameFromScript sub_08054C40
void DisplayTrack_SetFrameFromScript(void)
{
    struct DisplayTrack *track = sub_08054B80();
    if (track && Script_SeekToKeyword('p'))
        track->sprite.frame = Script_GetValue();
}
