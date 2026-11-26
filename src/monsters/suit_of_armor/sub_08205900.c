#include "global.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void SuitOfArmor_Update(void);
void SuitOfArmor_Destroy(void);
s32 SuitOfArmor_InitializeFromScript(void *, void *);
void *SuitOfArmor_Create(void *p)
{
    void *r = Task_Create(8, 0x198);
    if (r) {
        Task_SetCallbacks(r, SuitOfArmor_Update, SuitOfArmor_Destroy);
        if (SuitOfArmor_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
