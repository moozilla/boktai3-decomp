#include "global.h"
extern void *gUnk_03005420;
void Camera_ReadMoveRate(void *);
void sub_08226228(void);
void sub_08226658(void)
{
    Camera_ReadMoveRate(gUnk_03005420);
    sub_08226228();
}
