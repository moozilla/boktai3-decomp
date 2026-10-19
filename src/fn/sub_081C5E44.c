#include "global.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void *), void (*)(void *));
s32 ChallengeMenu_ScriptOpen(void *);
void Task_Kill(void *);
void ChallengeMenu_Update(void *);
void ChallengeMenu_Destroy(void *);
void *ChallengeMenu_CreateFromScript(void)
{
    void *p = Task_Create(8, 0x314);
    if (p != 0) {
        Task_SetCallbacks(p, ChallengeMenu_Update, ChallengeMenu_Destroy);
        if (ChallengeMenu_ScriptOpen(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
