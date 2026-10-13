// MATCHING_CONCESSION: register-pinning
#include "global.h"
void *sub_0803AD30(void);
void *Task_Create(int, int);
void Task_SetCallbacks(void *, void *, void *);
int sub_0803B370(void *);
int sub_0803B3B4(void *);
int sub_0803B40C(void *);
void Task_Kill(void *);
void *sub_0803B6B8(void)
{
    void *result = sub_0803AD30();
    register void *p asm("r4") = result;
    asm("" : : "r"(p));
    if (!p) {
        p = Task_Create(11, 0x930);
        if (p) {
            Task_SetCallbacks(p, sub_0803B370, sub_0803B3B4);
            if (sub_0803B40C(p) < 0) {
                Task_Kill(p);
                return 0;
            }
        }
        result = p;
    }
    return result;
}