#include "global.h"
int Script_GetKeywordValue(int, int);
int Script_SeekToKeyword(int);
void *Script_GetTextSourcePc(void);
int ResultTutorial_ReadScript(u8 *p)
{
    int result;
    *(u16 *)(p + 0x7a0) = Script_GetKeywordValue(0x65, 0);
    if (!Script_SeekToKeyword(0x6b) || !(*(void **)(p + 0x74) = Script_GetTextSourcePc()))
        result = -1;
    else
        result = 0;
    return result;
}
