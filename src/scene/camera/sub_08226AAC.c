#include "global.h"
struct S {
    u8 a[0x1e];
    u16 h;
};
void Camera_RunCompletionScript(struct S *);
void Camera_ConsumePendingMessage(struct S *);
void Camera_NotifyMoveComplete(struct S *p)
{
    u32 v = p->h;
    if (v == 1)
        Camera_RunCompletionScript(p);
    else if (v == 2)
        Camera_ConsumePendingMessage(p);
}
