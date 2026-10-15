#include "scene_actor_control.h"
#define SceneActorControl_Initialize sub_08067900
extern struct SceneActorControlManager *gUnk_02000128;
s32 SceneActorControl_Initialize(struct SceneActorControlManager *p)
{
    AnimData_Load(p->graphics, 0x1C1B);
    p->used = 0;
    p->sequenceResource = ScriptProc_Start(0x922E, 0x3DC2);
    gUnk_02000128 = p;
    return 0;
}
