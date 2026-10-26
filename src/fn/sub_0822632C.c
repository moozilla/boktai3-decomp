#include "global.h"
#include "ram.h"
struct S {
    u8 pad[0x1c];
    u16 f1c;
    u8 pad2[0x68 - 0x1e];
    u16 f68;
    u16 f6a;
    u16 f6c;
    u16 f6e;
    u16 f70;
};
void Camera_NotifyMoveComplete(struct S *);
void Camera_PanTo(s32 n, u16 *a)
{
    struct S *p = gActiveCamera;
    if (p) {
        p->f6c = a[0];
        p->f6e = a[1];
        p->f70 = a[2];
        p->f1c = 1;
        if (n <= 1) {
            p->f68 = 1;
            Camera_NotifyMoveComplete(p);
        } else {
            p->f6a = n;
            p->f68 = 2;
        }
    }
}
