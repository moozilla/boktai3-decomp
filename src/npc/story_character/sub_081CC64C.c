#include "npc/story_character/state_actor.h"
typedef void (*ActorStateCallback)(void *, struct StateActor *, u32);
/* Per-frame actor tick: clear the display flags, advance the frame counter,
 * run the update hook, then the current state's callback with the old count. */
s32 Actor_TickCallbacks(void *context, struct StateActor *actor)
{
    u32 frame;
    ActorStateCallback cb;
    actor->displayFlags = 0;
    actor->done = 0;
    frame = actor->stateFrame++;
    actor->animationCallback(actor);
    cb = (ActorStateCallback)actor->stateCallback;
    if (cb)
        cb(context, actor, frame);
    return 0;
}
