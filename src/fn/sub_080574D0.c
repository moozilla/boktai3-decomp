#include "level_advance_display.h"
struct LevelAdvanceManager *LevelAdvanceDisplay_CreateFromScript(void)
{
    struct LevelAdvanceManager *manager;
    if (gUnk_02000120)
        return gUnk_02000120;
    manager = Task_Create(9, 0x260);
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))LevelAdvanceDisplay_UpdateManager,
                          (void (*)(void *))LevelAdvanceDisplay_DestroyManager);
        if (LevelAdvanceDisplay_InitFromScript(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}
