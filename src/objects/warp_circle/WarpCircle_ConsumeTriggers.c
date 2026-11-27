#include "objects/warp_circle/warp_circle.h"
extern u8 *gSceneEventDispatchState;
u32 SceneMessages_SetPendingTrigger(void *, void *);
void WarpCircle_RaiseLift(struct WarpCircle *);
void WarpCircle_LowerLift(struct WarpCircle *);
void WarpCircle_ConsumeTriggers(struct WarpCircle *actor)
{
    u8 *receiver = actor->trigger;
    s32 i = 0;
    u8 **manager = &gSceneEventDispatchState;
    u8 *counts = (u8 *)actor + 0x1E;
    if (i >= counts[(*manager)[0x24]]) {
        goto done;
    }
    {
        u8 **cache = manager;
        s32 count;
        do {
            u8 **cache = manager;
            u8 **slot = cache;
            u8 *current = *slot;
            s32 offset = i * 4;
            u8 *base;
            u8 *message;
            offset += current[0x24] * 16;
            base = receiver + 8;
            base += offset;
            message = *(u8 **)base;
            SceneMessages_SetPendingTrigger(receiver, message);
            switch (message[6]) {
                case 5:
                    WarpCircle_SetUpdate(actor, WarpCircle_RaiseLift);
                    break;
                case 6:
                    WarpCircle_SetUpdate(actor, WarpCircle_LowerLift);
                    break;
            }
            i++;
            {
                u8 *current = *cache;
                u8 *counts = receiver + 6;
                u32 selected = current[0x24];
                counts += selected;
                count = *counts;
            }
        } while (i < count);
    }
done:;
}
