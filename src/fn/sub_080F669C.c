#define Sword_UpdateTask       sub_080F669C
#define Sword_DispatchState    sub_080F46D8
#define Sword_UpdateComponents sub_080F63C4
#include "global.h"
struct SwordOwner {
    u8 unknown0[0x1C];
    u32 flags;
};
struct SwordStatus {
    u32 unknown0, flags;
};
struct SwordTask {
    u8 unknown0[0x60];
    struct SwordOwner *owner;
    u8 unknown64[0x1A];
    s16 health;
    u8 unknown80[0xBA2];
    u16 flagsC22;
    u8 unknownC24[0x15C];
    u32 ticks;
    u8 unknownD84[4];
    struct SwordStatus status;
};
void sub_080F645C(struct SwordTask *);
void sub_080F46D8(void *);
void sub_080F63C4(void *);
s32 sub_080F669C(struct SwordTask *p)
{
    if (p->owner->flags & 4)
        return 1;
    sub_080F645C(p);
    if (p->health <= 0 || (p->flagsC22 & 4)) {
        struct SwordStatus *status = &p->status;
        u32 mask = 1;
        status->flags |= mask;
    } else {
        struct SwordStatus *status = &p->status;
        u32 mask = ~1;
        status->flags &= mask;
    }
    sub_080F46D8(p);
    sub_080F63C4(p);
    p->ticks++;
    return 0;
}
