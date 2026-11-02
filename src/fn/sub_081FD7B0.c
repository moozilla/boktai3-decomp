#include "global.h"
#include "sun_orb_effect.h"
void Sound_PlaySong(u32);
s32 OrbitEffect_UpdateExpansion(struct OrbitEffect *p, s32 slot)
{
    p->scale += 0x4000;
    if (p->scale > 0x40000) {
        Sound_PlaySong(0x564);
        SunOrb_SetUpdateCallback(&p->params, OrbitEffect_FireVolley);
    }
    return 0;
}
