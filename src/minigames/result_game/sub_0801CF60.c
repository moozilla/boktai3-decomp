#include "global.h"

void ResultMenu_DestroyGraphics(void *);

s32 ResultMenu_OnTaskExit(void *p)
{
    ResultMenu_DestroyGraphics(p);
    return 0;
}
