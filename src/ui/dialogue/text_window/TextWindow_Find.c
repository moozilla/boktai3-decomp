#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

struct TextWindow *TextWindowManager_FindWindow(struct WindowManager *, int);

struct TextWindow *TextWindow_Find(int id)
{
    struct WindowManager *manager = (void *)gWindowManager;

    if (!manager)
        return 0;
    return TextWindowManager_FindWindow(manager, id);
}
