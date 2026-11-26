#include "engine/video/display_track.h"
s32 DisplayTrack_InitManager(struct DisplayTrackManager *manager, u32 unused1, u32 unused2,
                             u32 capacity)
{
    manager->capacity = capacity;
    manager->count = 0;
    gDisplayTrackManager = manager;
    return 0;
}
