#include "scene_object_controller.h"
void SceneObject_UpdateRaised(struct SceneObjSlot *);
int SceneObjects_ScriptRaise(void)
{
    struct SceneObjPool *p = ResourceRegistry_GetResource(0x85e6);
    int found;
    int i;
    int id;
    if (!p)
        return -1;
    found = 0;
    if (!Script_SeekToKeyword(0x6e))
        return 0;
    id = Script_GetValue();
    for (i = 0; i < p->count; i++) {
        struct SceneObjSlot *s = &p->slots[i];
        if (s->id == id) {
            s->timer = 0;
            s->variant = SceneObjSlot_LiftVariant(SCENE_OBJ_LIFT_RAISED);
            s->callback = SceneObject_UpdateRaised;
            s->b39 = 0;
            s->b3a = 0;
            s->block->w6 &= ~3;
            s->block->w6 |= 2;
            s->block->b4 = (s->block->b4 & 0xf0) | s->b3b;
            s->lift = s->maxLift;
            s->spritePos.position.y = s->q0.position.y + s->maxLift;
            found = 1;
        }
    }
    return found;
}
