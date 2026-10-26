#include "global.h"
#include "ram.h"
void Camera_ReadMoveRate(void *);
void Camera_Reset(void);
void Camera_ResetFromScript(void)
{
    Camera_ReadMoveRate(gActiveCamera);
    Camera_Reset();
}
