#include "global.h"

struct S {
    u32 a, b, c;
};
extern struct S *gUnk_03001664;
struct S *Heap_Alloc(u32);
void Memory_Clear(struct S *, u32);
struct S *ScriptProc_Start(u32, u32);

s32 sub_08218BE0(void)
{
    if (gUnk_03001664 != 0)
        return 0;
    gUnk_03001664 = Heap_Alloc(12);
    if (gUnk_03001664 != 0) {
        struct S *p;
        Memory_Clear(gUnk_03001664, 12);
        p = ScriptProc_Start(0xA635, 0x3F51);
        if (p != 0) {
            *gUnk_03001664 = *p;
            gUnk_03001664->b += (u32)p;
            gUnk_03001664->c += (u32)p;
            return 0;
        }
    }
    return -1;
}
