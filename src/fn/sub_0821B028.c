#include "global.h"
s32 Script_GetValue(void);
void sub_0821AB78(void);
s32 sub_0821B028(void)
{
    if (Script_GetValue() == 0)
        sub_0821AB78();
    return 0;
}
