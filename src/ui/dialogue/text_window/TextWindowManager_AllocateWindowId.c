#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

struct TextWindow *TextWindowManager_FindWindow(struct WindowManager *, int);

int TextWindowManager_AllocateWindowId(struct WindowManager *manager)
{
    int i;

    for (i = 0; i < 4; i++) {
        manager->nextId++;
        if (manager->nextId > 999999)
            manager->nextId = 1;
        if (!TextWindowManager_FindWindow(manager, manager->nextId))
            return manager->nextId;
    }
    return -1;
}
