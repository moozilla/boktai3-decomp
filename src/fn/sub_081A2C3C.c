#include "global.h"
#include "ram.h"
struct E {
    u8 f[0x54];
};
void SceneActors_Destroy(void);
void Anim_Remove(void *);
void ScaledShadow_Destroy(void *);
void Collider_Remove(void *);
s32 Hresvelgr_Destroy(u8 *p)
{
    struct E *e;
    s32 i;
    s32 z;
    SceneActors_Destroy();
    Anim_Remove(p + 0x194);
    ScaledShadow_Destroy(p + 0x3b4);
    e = (struct E *)(p + 0x248);
    i = 1;
    do {
        Collider_Remove(e);
        e++;
        i--;
    } while (i >= 0);
    z = 0;
    gHresvelgr = z;
    return 0;
}
