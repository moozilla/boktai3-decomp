#include "display_track.h"
void DisplayTrack_SetPaletteTrackFromScript(void)
{
    struct DisplayTrack *track = DisplayTrack_FindFromScript();
    if (track && Script_SeekToKeyword('s')) {
        track->paletteStart = Script_GetValue();
        if (Script_SeekToKeyword('e')) {
            track->paletteEnd = Script_GetValue();
            if (Script_SeekToKeyword('f')) {
                track->paletteDuration = Script_GetValue();
                if (track->paletteDuration <= 1)
                    AnimData_SelectPaletteBank(&track->graphics,
                                               track->paletteBase + track->paletteEnd);
                else {
                    track->paletteFrame = 0;
                    track->flags |= 2;
                }
            }
        }
    }
}
