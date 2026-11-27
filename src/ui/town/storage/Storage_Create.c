#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 Storage_Init(void *);
void Task_Kill(void *);
void Storage_Update(void);
void Storage_Destroy(void);

void *Storage_Create(void)
{
    void *r = Task_Create(0xb, 6136);
    if (r != 0) {
        Task_SetCallbacks(r, Storage_Update, Storage_Destroy);
        if (Storage_Init(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
