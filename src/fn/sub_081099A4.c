#define MessagePanel_Update sub_081099A4
#include "global.h"
#include "engine.h"
struct MessagePanel {
    u8 unk0[0x20];
    void (*state)(struct MessagePanel *);
    const u8 *script;
    u32 unk28;
    u32 flags;
    s32 window;
    u8 mode;
    u8 entering;
    u16 timer;
    u32 unk38;
    u32 unk3C;
};
void Solar_PauseMeasurement(void);
extern u32 gUnk_0300546C;
extern u32 gUnk_03005408;
static inline u8 MessagePanel_IsBlocking(struct MessagePanel *p)
{
    if (p->flags & 1) return TRUE;
    return FALSE;
}
s32 sub_081099A4(struct MessagePanel *p)
{
    Solar_PauseMeasurement();
    gUnk_0300546C = 1;
    if (MessagePanel_IsBlocking(p)) gUnk_03005408 = 1;
    sub_0824923C(p,p->state);
    return 1;
}
