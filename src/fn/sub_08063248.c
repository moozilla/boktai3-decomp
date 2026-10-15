#include "scene_inventory_lifecycle.h"
#define SceneInventory_FinishAndCallScript sub_08063248
s32 SceneInventory_FinishAndCallScript(struct SceneInventoryLifecycle *p)
{
    if (p->frame > 31) {
        u32 script = p->completionScript;
        if (script != 0) {
            struct SceneInventoryScriptArgs args;
            struct SceneInventoryScriptCall call;
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
