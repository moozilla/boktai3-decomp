#include "global.h"
#include "scene_object.h"
s32 sub_0822B2F8(s32);
void AnimatedParticles_EmitDirectional(s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, void *);
void Scene_PlayMovementEffect(struct SceneObject *object)
{
    switch (object->type) {
        case 0:
            sub_0822B2F8(0x143);
            break;
        case 1:
            sub_0822B2F8(0x143);
            break;
        case 2:
        case 3:
        case 4:
        case 5:
        case 6:
            sub_0822B2F8(0x143);
            break;
        default:
            sub_0822B2F8(0x143);
            break;
    }
    if (sub_0800441C(object))
        AnimatedParticles_EmitDirectional(4, 1, 1, 0x40, 0, 255, 2, 4, 6, 6, &object->center);
    else
        Scene_SpawnEdgeParticles(&object->body.position, 0x90);
}
