#include "display_track.h"
#define DisplayTrack_CreateManagerFromScript sub_0805560C
u32 sub_08055578(struct DisplayTrackManager *);
s32 sub_080555B4(struct DisplayTrackManager *);
s32 sub_080555F8(struct DisplayTrackManager *, u32, u32, u32);
struct DisplayTrackManager *DisplayTrack_CreateManagerFromScript(u32 arg1, u32 arg2)
{
    if (gUnk_02000494)
        return gUnk_02000494;
    {
        struct DisplayTrackManager *manager;
        u32 capacity;
        if (Script_SeekToKeyword('m'))
            capacity = Script_GetValue();
        else
            capacity = 1;
        manager = Task_Create(9, capacity * sizeof(struct DisplayTrack) + 0x1C);
        if (manager) {
            Task_SetCallbacks(manager, (void (*)(void *))sub_08055578,
                              (void (*)(void *))sub_080555B4);
            if (sub_080555F8(manager, arg1, arg2, capacity) < 0) {
                Task_Kill(manager);
                return 0;
            }
        }
        return manager;
    }
}
