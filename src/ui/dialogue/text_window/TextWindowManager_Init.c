#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"



int TextWindowManager_Init(struct WindowManager *manager)
{
    gWindowManager = (void *)manager;
    manager->nextId = 0;
    manager->head = 0;
    return 0;
}
