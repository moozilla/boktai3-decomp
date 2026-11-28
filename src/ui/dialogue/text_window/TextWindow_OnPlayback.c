#include "global.h"
#include "engine/ram.h"
#include "ui/dialogue/text_window.h"
#include "ui/dialogue/text_window_manager.h"

void TextWindow_OnIdle(struct WindowManager *, struct TextWindow *);
void TextWindow_OnPlaybackComplete(void);
int TextState_StepPlayback(struct TextState *);

void TextWindow_OnPlayback(struct WindowManager *manager, struct TextWindow *window)
{
    struct TextState *text = &window->text;
    u32 i;

    for (i = 0; i <= window->enabled; i++) {
        if (text->pc) {
            text->entered = 0;
            if (TextState_StepPlayback(text)) {
                if (text->fresh)
                    window->callback = TextWindow_OnIdle;
                else
                    window->callback = TextWindow_OnPlaybackComplete;
                break;
            }
        }
    }
}
