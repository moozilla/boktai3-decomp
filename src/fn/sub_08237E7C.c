#include "global.h"

void *Task_Create(s32, s32);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
s32 SpriteBurst_Init(void *, s32, s32, s32);
void Task_Kill(void *);
void ActorGlyphTrail_UpdateTask(void);
void ActorGlyphTrail_DestroyTask(void);

void *ActorGlyphTrail_Create(s32 a, s32 b, s32 c)
{
    void *r = Task_Create(8, 0x140);
    if (r != 0) {
        Task_SetCallbacks(r, ActorGlyphTrail_UpdateTask, ActorGlyphTrail_DestroyTask);
        if (SpriteBurst_Init(r, a, b, c) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
