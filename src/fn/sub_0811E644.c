#include "global.h"
u8 *Modifier_GetTableTask(void);

s32 sub_0811E644(void)
{
    u8 *task = Modifier_GetTableTask();
    if (!task)
        return 0;
    return *(s32 *)(task + 0x1fc);
}
