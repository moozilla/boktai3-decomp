#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

void TextState_ClearSelectedChoices(struct TextState *);
void TextState_RunSelectedProcedures(struct TextState *);
int TextWindow_Refresh(int, int);
void sub_08249240(struct WindowManager *, struct TextWindow *, void *);

int TextWindowManager_OnUpdate(struct WindowManager *manager)
{
    struct TextWindow *window = manager->head;

    while (window) {
        struct TextWindow *previous = window->previous;

        struct TextState *text = &window->text;

        TextState_ClearSelectedChoices(text);
        if (window->symbol >= 0)
            TextWindow_Refresh(window->id, window->symbol);
        if (window->callback)
            sub_08249240(manager, window, window->callback);
        TextState_RunSelectedProcedures(text);
        window = previous;
    }
    return 0;
}
