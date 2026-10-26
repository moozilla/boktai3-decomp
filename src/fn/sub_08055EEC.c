#include "stage_icon_display.h"
/* The ROM returns the word its void callee leaves in r0 (saved lr); callers only test its sign. */
s32 StageIconDisplay_SelectUpdate(struct StageIconDisplay *, s32);
s32 StageIconDisplay_Initialize(struct StageIconDisplay *display, s32 variant, u32 option,
                                void (*completion)(void *, struct StageIconDisplay *),
                                void *argument)
{
    display->scriptCompletion = 0;
    display->completion = completion;
    display->completionArgument = argument;
    display->option = option;
    StageIconDisplay_InitAnimations(display, variant);
    {
        s32 status = StageIconDisplay_SelectUpdate(display, variant);
        gStageIconDisplay = display;
        display->revealedCount = 0;
        return status;
    }
}
