#include "global.h"
typedef struct {
    u8 pad[44];
    u16 rows[2][160];
    u16 active;
    s16 amplitude, delta;
    u16 start, end;
    u8 tail[0x1e];
    void (*noise)(void *);
} Screen;
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
extern void *gUnk_03004278, *gUnk_03003A00;
extern u16 gUnk_03004270, gUnk_03004274;

#define StatusScreen_Destroy sub_0800F1BC
extern void *gUnk_02000044;
// This callback preserves the BIOS wrapper's r0 result on return.
extern int CpuSetResult(const void *, void *, u32) asm("CpuSet");
int StatusScreen_Destroy(Screen *p)
{
    u32 zero = 0;
    int result = CpuSetResult(&zero, p->rows, 0x05000091);
    gUnk_03004278 = p->rows[p->active];
    gUnk_03003A00 = (void *)0x0400004C;
    gUnk_03004270 = 1;
    gUnk_03004274 = 0;
    gUnk_02000044 = 0;
    return result;
}
