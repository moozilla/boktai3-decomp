#include "global.h"
#include "system/save/region_list.h"
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
void Task_Kill(void *);
void Region_OnListUpdate(void *), Region_OnDestroy(void *);
s32 Region_InitFromScript(void *, u32, u32);
void *Task_Create(u32, u32);
void *Region_CreateFromScript(u32 a, u32 b)
{
    struct RegionList *list = Task_Create(4, sizeof(struct RegionList));

    if (list) {
        Task_SetCallbacks(list, Region_OnListUpdate, Region_OnDestroy);
        if (Region_InitFromScript(list, a, b) < 0) {
            Task_Kill(list);
            return 0;
        }
    }
    return list;
}
