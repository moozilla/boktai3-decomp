#include "game_start_menu.h"
void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void *, void *);
void Task_Kill(void *);
void GameStart_Update(void);
void GameStart_Destroy(void);

void *GameStart_Create(void)
{
    void *r = Task_Create(0xb, sizeof(struct GameStart));
    if (r != 0) {
        Task_SetCallbacks(r, GameStart_Update, GameStart_Destroy);
        if (GameStart_Init(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
