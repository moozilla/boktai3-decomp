#include "global.h"
typedef struct {
    s16 x, y, z;
} Vec;
void Collider_Init(void *, int, int, int, int, void *, void *);
void Collider_SetPosition(void *, void *, int);
void Collider_SetCallback(void *, void *, void *);
void sub_0821FF7C(void *, int, int, int);
s32 Collider_Refresh(void *);
void sub_080221D8(void);
extern u32 gUnk_030053F4;
void sub_0821FF58(void *, int, int, int, int, int);
void SceneActorGraphics_InitializeBody(u8 *p)
{
    Vec scale, rotation;
    u8 *drawable = p + 0x178;
    scale.x = 0x40;
    scale.y = 0x80;
    scale.z = 0x40;
    rotation.x = 0;
    rotation.y = 0x80;
    rotation.z = 0;
    Collider_Init(drawable, 0, 0x2001, 0, 16, &scale, &rotation);
    Collider_SetPosition(drawable, p + 0x260, 0);
    Collider_SetCallback(drawable, sub_080221D8, p);
    {
        int test = gUnk_030053F4 & 0x1000;
        int flags;
        flags = 0;
        if (test)
            flags = 0x40800;
        sub_0821FF58(drawable, *(s16 *)(p + 0x35c), *(s16 *)(p + 0x35e), 0, flags,
                     *(s16 *)(p + 0x360));
    }
}
