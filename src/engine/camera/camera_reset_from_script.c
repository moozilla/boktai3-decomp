#include "engine/shared/global.h"
#include "engine/shared/ram.h"
void Camera_ReadMoveRate(void *);
void Camera_Reset(void);
void Camera_ResetFromScript(void)
{
    Camera_ReadMoveRate(gActiveCamera);
    Camera_Reset();
}
