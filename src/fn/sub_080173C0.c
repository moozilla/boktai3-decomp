#include "global.h"
extern void *gUnk_0200007C;
int sub_0821ABA8(int, int);
void *sub_08219FBC(int, int);
void sub_0821A04C(void *, void *, void *);
int ScanlineWave_Update(void *);
int sub_080172D8(void *);
int ScanlineWave_Initialize(void *, int);
void sub_0821A0C0(void *);
void *ScanlineWave_Create(int id)
{
    void *p;
    if (gUnk_0200007C)
        return gUnk_0200007C;
    if (sub_0821ABA8('a', 0))
        p = sub_08219FBC(11, 628);
    else
        p = sub_08219FBC(9, 628);
    if (p) {
        sub_0821A04C(p, ScanlineWave_Update, sub_080172D8);
        if (ScanlineWave_Initialize(p, (u16)id) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
