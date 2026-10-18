#include "global.h"
void PlayStats_RemoveAnimations(void);
u32 RecordDisplay_OnTaskDestroy(void)
{
    PlayStats_RemoveAnimations();
    return 0;
}
