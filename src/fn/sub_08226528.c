#include "global.h"
extern u8 *gUnk_03005420;
struct Cam {
    u8 pad[0x1c];
    u16 mode;
    u16 timer;
    u8 pad2[0x48];
    u16 kind;
    u8 pad3[0x32];
    u32 a;
    u32 b;
};
void sub_08226528(u32 a, u32 b)
{
    struct Cam *camera = (struct Cam *)gUnk_03005420;
    if (camera != NULL) {
        camera->a = a;
        camera->b = b;
        camera->mode = 1;
        camera->kind = 7;
        camera->timer = 0;
    }
}
