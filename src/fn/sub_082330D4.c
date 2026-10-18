#define SpritePool_Initialize TransientSpritePool_Init
#define SpritePool_Destroy    TransientSpritePool_Destroy
#define gSpritePool           gUnk_020000AC
#include "global.h"
extern void *gUnk_020000AC;
void *Task_Create(u32, u32);
void Task_Kill(void *);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void TemporarySpritePool_Update(void);
void TransientSpritePool_Destroy(void);
s32 TransientSpritePool_Init(void *);
void *SpritePool_CreateSingleton(void)
{
    void *task = gUnk_020000AC;
    if (!task) {
        task = Task_Create(10, 0x4d8);
        if (task) {
            Task_SetCallbacks(task, TemporarySpritePool_Update, TransientSpritePool_Destroy);
            if (TransientSpritePool_Init(task) < 0) {
                Task_Kill(task);
                return 0;
            }
        }
    }
    return task;
}
