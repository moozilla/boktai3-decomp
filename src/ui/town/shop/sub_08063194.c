#include "ui/town/shop_lifecycle.h"
void Shop_ReadCompletionScript(struct ShopLifecycle *p)
{
    s32 result = Script_SeekToKeyword(0x65);
    if (result)
        p->completionScript = Script_GetValue();
    else
        p->completionScript = result;
}
