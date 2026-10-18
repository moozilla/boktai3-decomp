#define ProximityVolume_InitializeCallbacks sub_08180814
#define ProximityVolume_Update              sub_08180730
#define ProximityVolume_ClearState          sub_081806C4
#define ProximityVolume_IdleCallback        sub_08180810
#include "global.h"
struct ProximityVolume {
    void (*update)(void);
    void (*finish)(void);
};
u32 sub_081806C4(struct ProximityVolume *);
void sub_08180730(void);
void sub_08180810(void);
static inline void SetCallbacks(struct ProximityVolume *v, void (*a)(void), void (*b)(void))
{
    v->update = a;
    v->finish = b;
}
u32 sub_08180814(struct ProximityVolume *volume)
{
    u32 result;
    volume->update = 0;
    volume->finish = 0;
    result = sub_081806C4(volume);
    SetCallbacks(volume, sub_08180730, sub_08180810);
    return result;
}
