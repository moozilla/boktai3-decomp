#include "display_track.h"
#define DisplayTrack_HideFromScript sub_08054C64
void DisplayTrack_HideFromScript(void)
{
    struct DisplayTrack *track = sub_08054B80();
    if (track)
        track->sprite.flags |= 1;
}
