#include "display_track.h"
u32 DisplayTrack_TickManager(struct DisplayTrackManager *);
s32 DisplayTrack_DestroyManager(struct DisplayTrackManager *);
s32 DisplayTrack_InitManager(struct DisplayTrackManager *, u32, u32, u32);
struct DisplayTrackManager *DisplayTrack_CreateManagerFromScript(u32 arg1, u32 arg2)
{
    if (gDisplayTrackManager)
        return gDisplayTrackManager;
    {
        struct DisplayTrackManager *manager;
        u32 capacity;
        if (Script_SeekToKeyword('m'))
            capacity = Script_GetValue();
        else
            capacity = 1;
        manager = Task_Create(9, capacity * sizeof(struct DisplayTrack) + 0x1C);
        if (manager) {
            Task_SetCallbacks(manager, (void (*)(void *))DisplayTrack_TickManager,
                              (void (*)(void *))DisplayTrack_DestroyManager);
            if (DisplayTrack_InitManager(manager, arg1, arg2, capacity) < 0) {
                Task_Kill(manager);
                return 0;
            }
        }
        return manager;
    }
}
