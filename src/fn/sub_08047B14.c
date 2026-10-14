#include "background_flash.h"
s32 BackgroundFlash_Initialize(struct BackgroundFlashController *, void *, void *);
s32 BackgroundFlash_Update(struct BackgroundFlashController *);
s32 BackgroundFlash_Destroy(struct BackgroundFlashController *);
struct BackgroundFlashController *BackgroundFlash_Create(void *sourceA, void *sourceB)
{
    struct BackgroundFlashController *controller = Task_Create(9, sizeof(*controller));
    if (controller) {
        Task_SetCallbacks(controller, (void (*)(void *))BackgroundFlash_Update,
                          (void (*)(void *))BackgroundFlash_Destroy);
        if (BackgroundFlash_Initialize(controller, sourceA, sourceB) < 0) {
            Task_Kill(controller);
            return NULL;
        }
    }
    return controller;
}
