#define NamedObject_SnapTetherToPlatform sub_08103060
#define NamedObject_ClearAttachmentFlag  sub_081028F8
#define gNamedObjectTetherBounds         gUnk_086064B4
#include "global.h"
struct NamedObjectVector {
    s16 x, y, z, pad;
};
struct NamedObject {
    u8 pad0[0x58];
    struct NamedObjectVector offset;
    u8 pad60[0x58];
    s16 flags;
    u8 padBA[0x22];
    u32 name;
    u8 padE0[0x13];
    u8 kind;
    u8 padF4[0x18];
    struct NamedObjectVector position;
};
struct NamedObjectManager {
    u8 pad0[0x1374];
    u32 tetherName;
};
extern const struct NamedObjectVector gUnk_086064B4;
u32 TetherActor_CopyPosition(struct NamedObjectVector *);
void TetherActor_SetPosition(struct NamedObjectVector *);
void TetherActor_AddStatusFlags(u32, u32);
void sub_081028F8(u32, s32);
static inline void subtractVector(struct NamedObjectVector *d, struct NamedObjectVector *a,
                                  struct NamedObjectVector *b)
{
    d->x = a->x - b->x;
    d->y = a->y - b->y;
    d->z = a->z - b->z;
}
void sub_08103060(struct NamedObjectManager *manager, struct NamedObject *object)
{
    struct NamedObjectVector target, delta;
    if (object->kind == 2) {
        TetherActor_CopyPosition(&target);
        subtractVector(&delta, &target, &object->position);
        if (delta.x <= gUnk_086064B4.x && delta.x >= -gUnk_086064B4.x &&
            delta.z <= gUnk_086064B4.z && delta.z >= -gUnk_086064B4.z) {
            TetherActor_SetPosition(&object->position);
            sub_081028F8(manager->tetherName, 1);
            manager->tetherName = object->name;
            TetherActor_AddStatusFlags(3, object->name);
            {
                s16 mask = 16;
                object->flags = mask | object->flags;
            }
        }
    }
}
