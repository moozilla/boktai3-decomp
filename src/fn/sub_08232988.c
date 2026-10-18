#define ColliderSpritePool_Create     sub_08232988
#define ColliderSpritePool_Update     sub_082327E0
#define ColliderSpritePool_Destroy ScriptSpriteManager_Destroy
#define ColliderSpritePool_Initialize ScriptSpriteManager_Init
#include "global.h"
struct ColliderSpritePool;
void *Task_Create(u32, u32);
void Task_Kill(void *);
void Task_SetCallbacks(void *, void (*)(void), void (*)(void));
void sub_082327E0(void);
void ScriptSpriteManager_Destroy(void);
s32 ScriptSpriteManager_Init(struct ColliderSpritePool *, void *);
struct ColliderSpritePool *sub_08232988(void *argument)
{
    struct ColliderSpritePool *pool = Task_Create(8, 0x20);
    if (pool) {
        Task_SetCallbacks(pool, sub_082327E0, ScriptSpriteManager_Destroy);
        if (ScriptSpriteManager_Init(pool, argument) < 0) {
            Task_Kill(pool);
            return NULL;
        }
    }
    return pool;
}
