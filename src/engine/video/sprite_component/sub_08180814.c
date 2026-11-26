#include "global.h"
struct ProximityVolume {
    void (*update)(void);
    void (*finish)(void);
};
u32 SceneMotion_InitState(struct ProximityVolume *);
void ProximityVolume_Update(void);
void ProximityVolume_IdleCallback(void);
static inline void SetCallbacks(struct ProximityVolume *v, void (*a)(void), void (*b)(void))
{
    v->update = a;
    v->finish = b;
}
u32 ProximityVolume_InitializeCallbacks(struct ProximityVolume *volume)
{
    u32 result;
    volume->update = 0;
    volume->finish = 0;
    result = SceneMotion_InitState(volume);
    SetCallbacks(volume, ProximityVolume_Update, ProximityVolume_IdleCallback);
    return result;
}
