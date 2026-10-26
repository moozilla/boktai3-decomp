#include "global.h"
#include "ram.h"
struct S {
    u8 pad[0x1c];
    u16 f1c;
    u16 f1e;
    u8 pad2[0x68 - 0x20];
    u16 f68;
    u16 f6a;
};
void Camera_NotifyMoveComplete(struct S *);
void Camera_FollowPlayer(s32 n)
{
    struct S *p = gActiveCamera;
    if (p) {
        if (n <= 1) {
            p->f1c = 0;
            Camera_NotifyMoveComplete(p);
        } else {
            p->f1c = 1;
            p->f68 = 6;
            p->f6a = n;
        }
    }
}
