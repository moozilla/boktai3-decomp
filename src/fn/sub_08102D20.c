#include "global.h"
#include "scene_vec.h"
struct NamedObject {
    u8 pad0[0xBE];
    u16 limit;
    u8 padC0[0xE0 - 0xC0];
    u32 flags;
    u8 padE4[0xF3 - 0xE4];
    u8 kind;
    u8 padF4[0x10C - 0xF4];
    struct SceneVec position;
};
extern const struct SceneVec gUnk_086064B4;
extern const struct SceneVec gUnk_086064BC;
void NamedObject_ResolveAttachmentPosition(struct SceneVec *target, struct SceneVec *unused,
                                           struct NamedObject *object)
{
    struct SceneVec delta, bounds;
    delta.x = target->x - object->position.x;
    delta.y = target->y - object->position.y;
    delta.z = target->z - object->position.z;
    if (object->kind == 2)
        return;
    if (object->kind == 1 && !(object->flags & 8))
        bounds = gUnk_086064BC;
    else
        bounds = gUnk_086064B4;
    if (delta.x < 0) {
        if (delta.x < -bounds.x)
            delta.x = object->limit - bounds.x;
    } else if (delta.x > bounds.x) {
        delta.x = bounds.x - object->limit;
    }
    if (delta.y < 0) {
        if (delta.y < -bounds.y)
            delta.y = object->limit - bounds.y;
    } else if (delta.y > bounds.y) {
        delta.y = bounds.y - object->limit;
    }
    if (delta.z < 0) {
        if (delta.z < -bounds.z)
            delta.z = object->limit - bounds.z;
    } else if (delta.z > bounds.z) {
        delta.z = bounds.z - object->limit;
    }
    target->x = delta.x + object->position.x;
    target->y = delta.y + object->position.y;
    target->z = delta.z + object->position.z;
}
