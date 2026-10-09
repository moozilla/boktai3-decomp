#include "global.h"
extern u8 *gUnk_02000710;
struct SD394 { u8 f[0x4861]; u8 status; };
void sub_0816D394(struct SD394 *s) {
    u8 status = s->status;
    if (status == 0) {
        *(u16 *)(gUnk_02000710 + 0x610) = 1;
        *(u16 *)(gUnk_02000710 + 0x612) = status;
    } else {
        *(u16 *)(gUnk_02000710 + 0x610) = 0;
        *(u16 *)(gUnk_02000710 + 0x612) = 1;
    }
}
