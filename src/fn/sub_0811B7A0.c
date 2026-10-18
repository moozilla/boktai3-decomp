#include "global.h"
#define RegistryE7BC_QueueActiveObjects sub_0811B7A0
#define CategoryLists_EnqueueObject     sub_0811B2FC
struct E7BCObject {
    u8 unknown0[0x40];
    s32 parameter, active;
};
struct E7BCManager {
    u8 task[0x18];
    struct E7BCObject objects[12];
};
void sub_0811B2FC(struct E7BCObject *);
s32 sub_0811B7A0(struct E7BCManager *manager)
{
    struct E7BCObject *object = manager->objects;
    s32 i;
    for (i = 0; i < 12; i++) {
        if (manager->objects[i].active == 1)
            sub_0811B2FC(object);
        object++;
    }
    return 0;
}
