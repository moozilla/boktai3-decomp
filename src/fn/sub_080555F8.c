#include "display_track.h"
s32 DisplayTrack_InitManager(struct DisplayTrackManager *manager, u32 unused1, u32 unused2,
                             u32 capacity)
{
    u32 zero = 0;
    manager->capacity = capacity;
    manager->count = zero;
    gUnk_02000494 = manager;
    return 0;
}
