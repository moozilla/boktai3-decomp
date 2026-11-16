#include "global.h"
#include "vorn.h"

s32 Script_GetKeywordValue(s32, s32);

void Vorn_InitScriptCallback(struct Vorn *p)
{
    p->response = 0xffff;
    p->field67E = -1;
    p->field680 = -1;
    p->field684 = Script_GetKeywordValue(100, 0);
}
