#include "global.h"
int Solar_GetLevel(void);
void Callback_InvokeActor(void *, void *);
void sub_0801C24C(void *);
void Camera_SetPosition(void *);
int ResultMenu_Update(u8 *p)
{
    *(int *)(p + 0x50) = Solar_GetLevel();
    Callback_InvokeActor(p, *(void **)(p + 0x7a4));
    sub_0801C24C(p);
    Camera_SetPosition(p + 0x18);
    return 0;
}
