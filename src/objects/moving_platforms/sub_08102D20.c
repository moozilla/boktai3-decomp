#include "objects/moving_platforms/named_object.h"
#include "engine/math/scene_vec.h"
extern const struct SceneVec gUnk_086064B4;
extern const struct SceneVec gUnk_086064BC;
void MovingPlatform_ResolveAttachmentPosition(struct SceneVec *target, struct SceneVec *unused,
                                           struct NamedObject *object)
{
    struct SceneVec delta, bounds;
    delta.x = target->x - object->body.position.x;
    delta.y = target->y - object->body.position.y;
    delta.z = target->z - object->body.position.z;
    if (object->kind == 2)
        return;
    if (object->kind == 1 && !(object->field_E0 & 8))
        bounds = gUnk_086064BC;
    else
        bounds = gUnk_086064B4;
    if (delta.x < 0) {
        if (delta.x < -bounds.x)
            delta.x = object->speed - bounds.x;
    } else if (delta.x > bounds.x) {
        delta.x = bounds.x - object->speed;
    }
    if (delta.y < 0) {
        if (delta.y < -bounds.y)
            delta.y = object->speed - bounds.y;
    } else if (delta.y > bounds.y) {
        delta.y = bounds.y - object->speed;
    }
    if (delta.z < 0) {
        if (delta.z < -bounds.z)
            delta.z = object->speed - bounds.z;
    } else if (delta.z > bounds.z) {
        delta.z = bounds.z - object->speed;
    }
    target->x = delta.x + object->body.position.x;
    target->y = delta.y + object->body.position.y;
    target->z = delta.z + object->body.position.z;
}
