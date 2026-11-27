#include "monsters/pollen_flower.h"
s32 PollenFlower_UpdateAnimation(struct PollenFlower *);
s32 PollenFlower_Destroy(u8 *);
s32 PollenFlower_InitializeFromScript(void *, void *);
void *PollenFlower_CreateFromScript(void *p)
{
    void *r = Task_Create(8, sizeof(struct PollenFlower));
    if (r) {
        Task_SetCallbacks(r, (void (*)(void *))PollenFlower_UpdateAnimation,
                          (void (*)(void *))PollenFlower_Destroy);
        if (PollenFlower_InitializeFromScript(r, p) < 0) {
            Task_Kill(r);
            return 0;
        }
        return r;
    }
}
