#include "global.h"

void SelectionPanel_CommitSelection(u8 *);
void Registry3C9F_Close(u8 *);

void Registry3C9F_OnCloseRequested(u8 *p)
{
    u8 *flag = p + 0xD2C;

    if (*flag) {
        SelectionPanel_CommitSelection(p);
        Registry3C9F_Close(p);
        *flag = 0;
    }
}
