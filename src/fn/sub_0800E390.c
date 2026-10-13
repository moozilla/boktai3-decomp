#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
s32 PaletteSet_Initialize(void *, u16);
void sub_0821A0C0(void *);
void PaletteSet_Update(void);
void sub_0800E228(void);
void *sub_0800E390(u32 arg)
{
    void *obj;
    obj = sub_08219FBC(12, 0x2C8);
    if (obj != 0) {
        sub_0821A04C(obj, PaletteSet_Update, sub_0800E228);
        if (PaletteSet_Initialize(obj, arg) < 0) {
            sub_0821A0C0(obj);
            return 0;
        }
    }
    return obj;
}
