#include "global.h"
typedef struct {
    s16 x, y, z;
} Vec;
void sub_0821FEB4(void *, int, int, int, int, void *, void *);
void sub_0821FF24(void *, void *, int);
void sub_0821FF84(void *, void *, void *);
void sub_0821FF7C(void *, int, int, int);
void sub_0821FE40(void *);
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
    sub_0821FEB4(drawable, 0, 0x5101, 0, 16, &scale, &rotation);
    sub_0821FF24(drawable, p + 0x260, 0);
    sub_0821FF84(drawable, sub_08021F4C, p);
    sub_0821FF7C(drawable, resource, 0x802, 1);
    sub_0821FE40(drawable);
}
