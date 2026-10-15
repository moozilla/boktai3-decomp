#include "level_advance_display.h"
#define LevelAdvanceDisplay_CreateFromScript sub_080574D0
struct LevelAdvanceManager *LevelAdvanceDisplay_CreateFromScript(void)
{
    struct LevelAdvanceManager *manager;
    if (gUnk_02000120)
        return gUnk_02000120;
    manager = Task_Create(9, 0x260);
    if (manager) {
        Task_SetCallbacks(manager, (void (*)(void *))sub_0805732C, (void (*)(void *))sub_080573C8);
        if (sub_08057480(manager) < 0) {
            Task_Kill(manager);
            return 0;
        }
    }
    return manager;
}
