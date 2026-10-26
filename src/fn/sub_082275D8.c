#include "global.h"
#include "ram.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 Camera_Initialize(void *, void *);
void Task_Kill(void *);
void Camera_Update(void);
void Camera_Destroy(void);

void *Camera_Create(void *a)
{
    void *p;
    if (gActiveCamera != 0)
        return gActiveCamera;
    p = Task_Create(7, 0xA4);
    if (p != 0) {
        Task_SetCallbacks(p, Camera_Update, Camera_Destroy);
        if (Camera_Initialize(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
