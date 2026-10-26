#include "display_track.h"
#include "render.h"
void DisplayTrack_Update(struct DisplayTrack *);
u32 DisplayTrack_TickManager(struct DisplayTrackManager *manager)
{
    s32 i = 0;
    if (i < manager->count) {
        struct DisplayTrack *track = manager->entries;
        do {
            if (!(gEnginePauseFlags != 0 && track->runDuringPause == 0))
                DisplayTrack_Update(track);
            track++;
            i++;
        } while (i < manager->count);
    }
    return 0;
}
