#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 GroupObjects_Initialize(void *, void *);
void sub_0821A0C0(void *);
void GroupObjects_Update(void);
void GroupObjects_Destroy(void);
void *sub_080111A8(void *arg)
{
    void *obj;
    obj = sub_08219FBC(11, 0xD08);
    if (obj != 0) {
        sub_0821A04C(obj, GroupObjects_Update, GroupObjects_Destroy);
        if (GroupObjects_Initialize(obj, arg) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
