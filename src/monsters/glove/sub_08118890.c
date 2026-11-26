#include "global.h"
#include "engine/engine.h"
#include "engine/ram.h"
#include "monsters/glove/glove.h"
void SceneMessages_RegisterReceiver(void *, u32, u32);
void Glove_RegisterMessageReceiver(struct Glove *s)
{
    u32 m = 0x800;
    if (((gEngineFlags | gScenePauseFlags) & m) == 0) {
        SceneMessages_RegisterReceiver(&s->queue, s->nextScript, 0xb);
    }
}
