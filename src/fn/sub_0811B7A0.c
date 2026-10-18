#include "global.h"
struct E7BCObject {
    u8 unknown0[0x40];
    s32 parameter, active;
};
struct E7BCManager {
    u8 task[0x18];
    struct E7BCObject objects[12];
};
void SpatialIndex_InsertEntry(struct E7BCObject *);
s32 RegistryE7BC_QueueActiveObjects(struct E7BCManager *manager)
{
    struct E7BCObject *object = manager->objects;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (manager->objects[i].active == 1)
            SpatialIndex_InsertEntry(object);
        object++;
    }
    return 0;
}
