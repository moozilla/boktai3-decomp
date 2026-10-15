#include "global.h"
typedef struct {
    s16 x, y, z;
} Vec;
void Collider_Init(void *, int, int, int, int, void *, void *);
void Collider_SetPosition(void *, void *, int);
void Collider_SetCallback(void *, void *, void *);
void sub_0821FF7C(void *, int, int, int);
s32 Collider_Refresh(void *);
void sub_08021F4C(void);
void SceneActorGraphics_InitializeOverlay(u8 *p, int resource)
{
    Vec scale, rotation;
    u8 *drawable = p + 0x1cc;
    scale.x = 0x60;
    scale.y = 0x80;
    scale.z = 0x60;
    rotation.x = 0;
    rotation.y = 0x80;
    rotation.z = 0;
    Collider_Init(drawable, 0, 0x5101, 0, 16, &scale, &rotation);
    Collider_SetPosition(drawable, p + 0x260, 0);
    Collider_SetCallback(drawable, sub_08021F4C, p);
    sub_0821FF7C(drawable, resource, 0x802, 1);
    Collider_Refresh(drawable);
}
