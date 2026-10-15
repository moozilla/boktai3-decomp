#include "display_track.h"
void DisplayTrack_SetPriorityFromScript(void)
{
    struct DisplayTrack *track = DisplayTrack_FindFromScript();
    if (track && Script_SeekToKeyword('p'))
        track->sprite.priority = Script_GetValue();
}
