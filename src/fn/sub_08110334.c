#include "global.h"
struct S {
    u8 f[0xFE4];
    void *fp;
    u8 p[0x18];
    u8 st;
    u8 q[0x23];
    u8 c;
};
u8 Save_ToggleSelectedSlot(void);
void sub_08110378(void);
void SaveMenu_UpdateSaveFailure(void);
static inline void set(struct S *s, void *fp, u8 st)
{
    s->fp = fp;
    s->c = 1;
    s->st = st;
}
void SaveMenu_UpdateSlotCommit(struct S *s)
{
    if (Save_ToggleSelectedSlot())
        set(s, sub_08110378, 7);
    else
        set(s, SaveMenu_UpdateSaveFailure, 8);
}
