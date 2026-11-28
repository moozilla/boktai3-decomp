#include "npc/story_character/state_actor.h"

u32 SceneMessages_UnregisterReceiver(u8 *);

u32 Kuro_DispatchReceiverCallback(void *ctx, struct StateActor *actor)
{
    return SceneMessages_UnregisterReceiver(actor->receiver);
}
