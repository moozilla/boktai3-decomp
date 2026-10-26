#include "scene_actor_control.h"
#include "ram.h"
s32 SceneActorControl_Initialize(struct SceneActorControlManager *p)
{
    AnimData_Load(p->graphics, 0x1C1B);
    p->used = 0;
    p->sequenceResource = ScriptProc_Start(0x922E, 0x3DC2);
    gSceneActorControlManager = p;
    return 0;
}
