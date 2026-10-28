#include "global.h"
struct P {
    u32 a, b;
};
struct S {
    u8 pad[0x1c];
    u16 f1c;
    u8 pad2[0x68 - 0x1e];
    u16 f68;
    u16 f6a;
    u8 pad3[0x74 - 0x6c];
    struct P f74;
    u16 f7c;
};
extern struct S *gActiveCamera;
s32 SceneBody_GetById(u32);
void Camera_NotifyMoveComplete(struct S *);
void Camera_PanToActor(u32 a, s32 n, struct P *b)
{
    if (gActiveCamera) {
        gActiveCamera->f7c = a;
        if (SceneBody_GetById(gActiveCamera->f7c)) {
            struct S *p = gActiveCamera;
            p->f1c = 1;
            p->f74 = *b;
            if (n <= 1) {
                p->f68 = 3;
                Camera_NotifyMoveComplete(p);
            } else {
                p->f6a = n;
                p->f68 = 4;
            }
        }
    }
}
