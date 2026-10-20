#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 Password_InitializeEntryMenu(void *);
void Task_Kill(void *);
void Password_UpdateEntryMenu(void);
void Password_DestroyEntryMenu(void);

void *Password_CreateEntryMenu(void)
{
    void *r = Task_Create(0xb, 2076);
    if (r != 0) {
        Task_SetCallbacks(r, Password_UpdateEntryMenu, Password_DestroyEntryMenu);
        if (Password_InitializeEntryMenu(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
