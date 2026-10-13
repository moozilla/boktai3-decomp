#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 ResultMenu_Initialize(void *, void *, void *);
void sub_0821A0C0(void *);
void ResultMenu_Update(void);
void sub_0801CF60(void);
void *sub_0801D0BC(void *a, void *b)
{
    void *obj;
    obj = sub_08219FBC(11, 0x7F4);
    if (obj != 0) {
        sub_0821A04C(obj, ResultMenu_Update, sub_0801CF60);
        if (ResultMenu_Initialize(obj, a, b) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
