#include "cell_gate.h"
#define CellGate_UpdatePassageBarrier sub_08050280
s32 sub_080501E8(struct CellGate *);
void sub_08050280(struct CellGate *p)
{
    s32 r = sub_08050188(p);
    if (r != 0) {
        if (sub_08050128(p) != 0 && (r = sub_080501E8(p)) == 0) {
            if (p->registered != 0) {
                sub_0821D6D0(p->registration);
                p->registered = r;
                sub_08158CD8(gUnk_02000580);
                if (p->scriptId != 0)
                    Script_Call(p->scriptId, 0);
            }
        } else if (p->registered == 0) {
            sub_0821D698(p->registration, p->cell, 0, p->height, 0xff, 0x82a);
            p->registered = 1;
        }
    } else if (p->registered != 0) {
        sub_0821D6D0(p->registration);
        p->registered = r;
    }
}
