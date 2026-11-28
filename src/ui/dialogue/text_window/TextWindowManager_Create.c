#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

void *Task_Create(int, int);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
void Task_Kill(void *);
int TextWindowManager_OnUpdate(struct WindowManager *);
int TextWindowManager_OnDestroy(struct WindowManager *);
int TextWindowManager_Init(struct WindowManager *);

struct WindowManager *TextWindowManager_Create(void)
{
    struct WindowManager *manager = (void *)gWindowManager;

    if (!manager) {
        manager = Task_Create(12, sizeof(struct WindowManager));
        if (manager) {
            Task_SetCallbacks(manager, (void (*)(void *))TextWindowManager_OnUpdate,
                              (void (*)(void *))TextWindowManager_OnDestroy);
            if (TextWindowManager_Init(manager) < 0) {
                Task_Kill(manager);
                return 0;
            }
        }
    }
    return manager;
}
