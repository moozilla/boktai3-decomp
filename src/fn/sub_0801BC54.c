#include "global.h"
void ScreenTransition_SendCommand(int, int, int, int, int, int, int);
void Transformation_ReportResult(void *);
void Task_Kill(void *);
void Transformation_Finish(u8 *p)
{
    if (*(u32 *)(p + 0x3c) == 60)
        ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xffff, 0);
    if (*(u32 *)(p + 0x3c) > 91) {
        if (*(u16 *)(p + 0x7a0))
            Transformation_ReportResult(p);
        Task_Kill(p);
    } else
        (*(u32 *)(p + 0x3c))++;
}
