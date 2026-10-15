#include "display_track.h"
#define DisplayTrack_TickManager sub_08055578
extern u32 gUnk_0300523C;
void sub_08054EC0(struct DisplayTrack *);
u32 DisplayTrack_TickManager(struct DisplayTrackManager *manager)
{
    s32 i = 0;
    if (i < manager->count) {
        struct DisplayTrack *track = manager->entries;
        do {
            if (!(gUnk_0300523C != 0 && track->runDuringPause == 0))
                sub_08054EC0(track);
            track++;
            i++;
        } while (i < manager->count);
    }
    return 0;
}
