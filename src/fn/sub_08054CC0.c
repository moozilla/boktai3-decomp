#include "display_track.h"
#define DisplayTrack_SetPaletteTrackFromScript sub_08054CC0
void DisplayTrack_SetPaletteTrackFromScript(void)
{
    struct DisplayTrack *track = sub_08054B80();
    if (track && Script_SeekToKeyword('s')) {
        track->paletteStart = Script_GetValue();
        if (Script_SeekToKeyword('e')) {
            track->paletteEnd = Script_GetValue();
            if (Script_SeekToKeyword('f')) {
                track->paletteDuration = Script_GetValue();
                if (track->paletteDuration <= 1)
                    AnimData_SelectTiles(&track->graphics, track->paletteBase + track->paletteEnd);
                else {
                    track->paletteFrame = 0;
                    track->flags |= 2;
                }
            }
        }
    }
}
