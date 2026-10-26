#include "global.h"
#include "ram.h"
void *Task_Create(u32, u32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 SpritePool_Initialize(void *);
void Task_Kill(void *);
void SpritePool_Update(void);
void SpritePool_Destroy(void);

void *SpritePool_Create(void)
{
    void *p = (void *)gSpritePoolManager;
    if (p == 0) {
        p = Task_Create(10, 0xb1c);
        if (p != 0) {
            Task_SetCallbacks(p, SpritePool_Update, SpritePool_Destroy);
            if (SpritePool_Initialize(p) < 0) {
                Task_Kill(p);
                return 0;
            }
        }
    }
    return p;
}
