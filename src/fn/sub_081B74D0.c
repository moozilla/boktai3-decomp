#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 VanargandAfterimagePool_Init(void *);
void Task_Kill(void *);
void VanargandAfterimagePool_Update(void);
void VanargandAfterimagePool_Destroy(void);

void *VanargandAfterimagePool_Create(void)
{
    void *r = Task_Create(9, 0x27c);
    if (r != 0) {
        Task_SetCallbacks(r, VanargandAfterimagePool_Update, VanargandAfterimagePool_Destroy);
        if (VanargandAfterimagePool_Init(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
