#include "ui/town/shop_lifecycle.h"
s32 Shop_FinishAndCallScript(struct ShopLifecycle *p)
{
    if (p->frame > 31) {
        u32 script = p->completionScript;
        if (script != 0) {
            struct ShopScriptArgs args;
            struct ShopScriptCall call;
            call.argumentCount = 2;
            args.purchased = p->purchased;
            args.sold = p->sold;
            call.args = &args;
            Script_Call(script, &call);
        }
        Task_Kill(p);
    }
    return 0;
}
