#include "global.h"
#include "ram.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 sub_081625C0(void *, void *, void *, void *);
void Task_Kill(void *);
void sub_0816233C(void);
void PlayerAttackEffect_Destroy(void);

void *PlayerAttackEffect_Create(void *a, void *b, void *c)
{
    void *r;
    if (gPlayerAttackEffect != 0)
        return gPlayerAttackEffect;
    r = Task_Create(0xa, 0x5A8);
    if (r != 0) {
        Task_SetCallbacks(r, sub_0816233C, PlayerAttackEffect_Destroy);
        if (sub_081625C0(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    gPlayerAttackEffect = r;
    return r;
}
