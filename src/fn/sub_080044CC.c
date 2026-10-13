#include "global.h"
#include "scene_object.h"
void AnimatedParticles_EmitDirectional(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32,
                                       struct SceneVec *);
void Scene_PlayNeighborMovementEffect(struct SceneObject *object)
{
    struct SceneVec position;
    s32 angle;
    if (Scene_CheckNeighborHeight(object, object->cellRef.index, object->direction)) {
        position = object->center;
        angle = 0;
        switch (object->direction) {
            case 0:
                position.z -= 128;
                angle = 0xB0;
                break;
            case 1:
                position.x += 128;
                angle = -16;
                break;
            case 2:
                position.z += 128;
                angle = 0x30;
                break;
            case 3:
                position.x -= 128;
                angle = 0x70;
                break;
        }
        AnimatedParticles_EmitDirectional(2, 1, 1, 0x40, angle, 31, 2, 4, 6, 6, &position);
        sub_0800435C(object->type);
    }
}
