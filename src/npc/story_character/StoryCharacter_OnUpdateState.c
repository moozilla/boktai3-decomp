#include "npc/story_character/scene_character.h"

u32 StoryCharacter_OnUpdateState(void *context, struct SceneNpc *npc)
{
    u32 frame;
    void (*callback)(void *, struct SceneNpc *, u32);
    npc->displayFlags = 0;
    npc->done = 0;
    frame = npc->stateFrame++;
    callback = (void *)npc->stateCallback;
    if (callback)
        callback(context, npc, frame);
    return 0;
}
