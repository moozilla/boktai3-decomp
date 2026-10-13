#include "group_objects.h"

void *sub_080111A8(void *arg)
{
    void *obj;
    obj = Task_Create(11, 0xD08);
    if (obj != 0) {
        Task_SetCallbacks(obj, GroupObjects_Update, GroupObjects_Destroy);
        if (GroupObjects_Initialize(obj, (int)arg) < 0) {
            Task_Kill(obj);
            return 0;
        }
    }
    return obj;
}
