#include "monsters/cannons/wind_cannon.h"
s32 WindCannonManager_Init(void *);
u32 WindCannonManager_Update(struct FanCannonManager *);
u32 WindCannonManager_Destroy(struct FanCannonManager *);

void *WindCannonManager_Create(void)
{
    void *p = Task_Create(8, sizeof(struct FanCannonManager));
    if (p) {
        Task_SetCallbacks(p, (void (*)(void *))WindCannonManager_Update,
                          (void (*)(void *))WindCannonManager_Destroy);
        if (WindCannonManager_Init(p) < 0) {
            Task_Kill(p);
            return 0;
        }
        return p;
    }
}
