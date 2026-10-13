#include "scanline_presentation.h"

void *sub_0800E08C(void)
{
    struct ScanlinePresentation *p = Task_Create(8, 0x52C);
    if (p) {
        Task_SetCallbacks(p, (void *)ScanlinePresentation_Update, (void *)ScanlinePresentation_Destroy);
        if (sub_0800DC90(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
