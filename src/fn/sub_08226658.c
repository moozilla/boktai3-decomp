#include "global.h"
extern void *gUnk_03005420;
void Camera_ReadMoveRate(void *);
void Camera_Reset(void);
void sub_08226658(void)
{
    Camera_ReadMoveRate(gUnk_03005420);
    Camera_Reset();
}
