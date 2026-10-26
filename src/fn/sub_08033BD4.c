#include "global.h"
#include "engine.h"
#include "ram.h"
s32 TextWindowManager_OnUpdate(void *);
s32 TextWindowManager_OnDestroy(void *);
s32 TextWindowManager_Init(void *);
void *TextWindowManager_Create(void)
{
    void *p = gWindowManager;
    if (!p) {
        p = Task_Create(12, 0x20);
        if (p) {
            Task_SetCallbacks(p, (void (*)(void *))TextWindowManager_OnUpdate,
                              (void (*)(void *))TextWindowManager_OnDestroy);
            if (TextWindowManager_Init(p) < 0) {
                Task_Kill(p);
                return 0;
            }
        }
    }
    return p;
}
