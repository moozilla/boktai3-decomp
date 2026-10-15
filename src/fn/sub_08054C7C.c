#include "display_track.h"
#define DisplayTrack_ShowFromScript sub_08054C7C
void DisplayTrack_ShowFromScript(void)
{
    struct DisplayTrack *track = sub_08054B80();
    if (track)
        track->sprite.flags &= ~1;
}
