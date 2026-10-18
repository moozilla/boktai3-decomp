#include "global.h"
#define MovingSprite_Update  MovingGlyph_UpdateTask
#define MovingSprite_Destroy MovingGlyph_DestroyTask
void *Task_Create(u32, u32);
void Task_Kill(void *);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void MovingGlyph_UpdateTask(void);
void MovingGlyph_DestroyTask(void);
s32 MovingSprite_Initialize(void *, void *, u32, u32, u32, u32, u32, u32);
void *MovingSprite_Create_082375FC(void *position, u32 id, u32 a, u32 direction, u32 duration,
                                   u32 delay, u32 b)
{
    void *p = Task_Create(10, 0x7C);
    if (p) {
        Task_SetCallbacks(p, MovingGlyph_UpdateTask, MovingGlyph_DestroyTask);
        if (MovingSprite_Initialize(p, position, id, a, direction, duration, delay, b) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}
