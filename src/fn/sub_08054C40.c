#include "display_track.h"
void DisplayTrack_SetFrameFromScript(void)
{
    struct DisplayTrack *track = DisplayTrack_FindFromScript();
    if (track && Script_SeekToKeyword('p'))
        track->sprite.frame = Script_GetValue();
}
