#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
s32 Player_InitializeFromScript(void *, void *, void *);
void Task_Kill(void *);
void Player_Update(void);
void Player_Destroy(void);

void *Player_Create(void *a, void *b)
{
    void *r = Task_Create(6, 0xCAC);
    if (r != 0) {
        Task_SetCallbacks(r, Player_Update, Player_Destroy);
        if (Player_InitializeFromScript(r, a, b) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
