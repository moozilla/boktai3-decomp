#include "global.h"
typedef struct {
    u32 pad[2], flags;
} Graphic;
u8 *sub_0822D6C8(u32);
void sub_0822D764(u8 *, void *);
void sub_08030C84(u32);
void sub_08174480(void *, void *);
void sub_081744AC(u8 *, u32);
u32 sub_08174594(u8 *);
void Anim_LoadKeyframe(u8 *, u8 *, u32, u32);
void sub_08030BF8(void);
void sub_08033468(void);
void sub_0803343C(u32);
void Presentation_UpdateActorGraphic(u8 *p, u32 fallback)
{
    u8 *actor = sub_0822D6C8(p[0x1d]);
    u8 data[32];
    if (*actor != 255) {
        sub_0822D764(actor, data);
        sub_08030C84(*(u32 *)(p + 0xda8));
        sub_08174480(data, p + 0x44);
        sub_081744AC(actor, *(u32 *)(p + 0xda8));
        {
            u32 resource = sub_08174594(actor);
            ((Graphic *)(p + 0x5b4))->flags &= ~1;
            Anim_LoadKeyframe(p + 0x5b4, p + 0x6c, (u16)resource, 0);
        }
    } else {
        sub_08030BF8();
        if (!fallback)
            sub_08033468();
        else
            sub_0803343C(fallback);
        ((Graphic *)(p + 0x5b4))->flags |= 1;
    }
}
