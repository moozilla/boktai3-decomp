#include "global.h"
#include "ui/books/bestiary_display.h"

void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081D4CE0(struct BestiaryDisplay *, u32);
void Task_Kill(void *);
void BestiaryDisplay_UpdateEntry(void);
void BestiaryDisplay_OnTaskDestroy(void);

struct BestiaryDisplay *BestiaryDisplay_Create(u32 arg)
{
    struct BestiaryDisplay *p = Task_Create(8, sizeof(struct BestiaryDisplay));

    if (p) {
        Task_SetCallbacks(p, BestiaryDisplay_UpdateEntry, BestiaryDisplay_OnTaskDestroy);
        if (sub_081D4CE0(p, arg) < 0) {
            Task_Kill(p);
            return NULL;
        }
    }
    return p;
}
