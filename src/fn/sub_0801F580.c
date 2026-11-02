#include "scene_object_slot.h"
void Camera_RequestShake(int);
void SceneObject_UpdateRaised(struct SceneObjSlot *s)
{
    s16 timer = s->timer;
    if (timer) {
        Camera_RequestShake((s16)s->timer);
        s->timer--;
    } else if (s->raisedScript) {
        Script_Call(s->raisedScript, 0);
        s->raisedScript = timer;
    }
    s->pad0[7] = 2;
}
