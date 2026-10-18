#include "global.h"
struct S {
    u8 f[0x2e8];
    void *arr[4];
    void *a2f8;
    u32 pd;
    u32 st;
};
u8 ChallengeMenu_ConsumeStateEntry(void *);
void Script_Call(void *, u32);
void Task_Kill(void *);
void ChallengeMenu_DispatchSelection(struct S *p)
{
    if (ChallengeMenu_ConsumeStateEntry(p) != 0) {
        u32 s = p->st;
        if (s == 5) {
            Script_Call(p->a2f8, 0);
            Task_Kill(p);
        } else if (s <= 3) {
            void *q = p->arr[s];
            if (q != 0) {
                Script_Call(q, 0);
                Task_Kill(p);
            }
        }
    }
}
