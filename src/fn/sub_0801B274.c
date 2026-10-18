#include "global.h"
int SelectionMenu_ConsumeCallbackFlag(void *);
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void SelectionMenu_Hide(void *);
void ScrollingDialogue_RequestHide(void);
void SelectionMenu_CallConfirmScript(void *);
void Task_Kill(void *);
void SelectionMenu_Confirm(u8 *p)
{
    if (SelectionMenu_ConsumeCallbackFlag(p))
        ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xffff, 0);
    if (*(u32 *)(p + 0x28) > 31) {
        SelectionMenu_Hide(p);
        ScrollingDialogue_RequestHide();
        if (*(u16 *)(p + 0x42))
            SelectionMenu_CallConfirmScript(p);
        Task_Kill(p);
    } else
        (*(u32 *)(p + 0x28))++;
}
