#include "objects/sliding_doors/scene_object_controller.h"
void SlidingDoors_UpdateRaised(struct SceneObjSlot *);
int SlidingDoors_ScriptRaise(void)
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
            s->callback = SlidingDoors_UpdateRaised;
            s->pose = 0;
            s->poseFrame = 0;
            s->terrain->cell.flags &= ~3;
            s->terrain->cell.flags |= 2;
            s->terrain->cell.shapeAndHeight = (s->terrain->cell.shapeAndHeight & 0xf0) | s->cellHeight;
            s->lift = s->maxLift;
            s->sprite.position.y = s->q0.position.y + s->maxLift;
            found = 1;
        }
    }
    return found;
}
