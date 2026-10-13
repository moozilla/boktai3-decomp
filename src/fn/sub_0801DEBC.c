#include "global.h"
int sub_0821ABA8(int, int);
int Script_SeekToKeyword(int);
void *sub_08227E90(void);
int ResultTutorial_ReadScript(u8 *p)
{
    int result;
    *(u16 *)(p + 0x7a0) = sub_0821ABA8(0x65, 0);
    if (!Script_SeekToKeyword(0x6b) || !(*(void **)(p + 0x74) = sub_08227E90()))
        result = -1;
    else
        result = 0;
    return result;
}
