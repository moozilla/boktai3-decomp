#include "display_track.h"
void DisplayTrack_HideFromScript(void)
{
    struct DisplayTrack *track = DisplayTrack_FindFromScript();
    if (track)
        track->sprite.flags |= 1;
}
