#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 MessageMenu_Initialize(void *);
void sub_0821A0C0(void *);
void MessageMenu_Update(void);
void MessageMenu_Destroy(void);
void *sub_080114CC(void)
{
    void *obj;
    obj = sub_08219FBC(11, 0x4D8);
    if (obj != 0) {
        sub_0821A04C(obj, MessageMenu_Update, MessageMenu_Destroy);
        if (MessageMenu_Initialize(obj) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
