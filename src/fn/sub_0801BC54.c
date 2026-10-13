#include "global.h"
void sub_082279A8(int, int, int, int, int, int, int);
void Transformation_ReportResult(void *);
void sub_0821A0C0(void *);
void Transformation_Finish(u8 *p)
{
    if (*(u32 *)(p + 0x3c) == 60)
        sub_082279A8(1, 5, 4, 4, 4, 0xffff, 0);
    if (*(u32 *)(p + 0x3c) > 91) {
        if (*(u16 *)(p + 0x7a0))
            Transformation_ReportResult(p);
        sub_0821A0C0(p);
    } else
        (*(u32 *)(p + 0x3c))++;
}
