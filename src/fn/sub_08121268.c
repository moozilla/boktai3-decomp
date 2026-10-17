#include "global.h"
#include "engine.h"
struct DrawFlags {
    u8 pad0[0x16];
    u16 flags;
};
struct A8CBObject {
    u8 pad0[0x10];
    u8 block10[0x3B];
    u8 field4B;
    u8 pad4C[0x1C];
    u8 block68[0x48];
    u8 blockB0[0x50C];
    u8 block5BC[0x3C];
    u8 anim[0x20];
    u8 block618[0x234];
    u32 field84C;
    u8 active;
    u8 pad851[3];
    void (*update)(void);
    u8 pad858[0x34];
    struct DrawFlags draw;
    u8 pad8A4[6];
    u8 field8AA;
    u8 pad8AB[9];
    u32 field8B4;
};
extern void *gUnk_0200020C;
void *sub_08121210(void);
struct A8CBObject *sub_08120264(void *manager);
void sub_081200C8(struct A8CBObject *);
void sub_0811C9F8(struct A8CBObject *, void *, s32);
void sub_0811C89C(void *, void *, u8);
void sub_08112BA8(void *);
void sub_0811FDB4(struct A8CBObject *);
void sub_081202CC(struct A8CBObject *);
void sub_0811EDC0(void *, void *, s32, s32);
void sub_08120B44(void);
/* Registry routine A8CB: takes an object from the manager at 0x0200020C
   (created on demand), reads its options (sub_081200C8: -n, -p, -s, -P),
   loads sprite 0x3D99 and starts it with update sub_08120B44 (tentative). */
void RegistryA8CB_CreateFromScript(void)
{
    void *manager = gUnk_0200020C;
    struct A8CBObject *e;
    void *anim;
    u16 one;
    if (!manager) {
        manager = sub_08121210();
        if (!manager)
            return;
    }
    e = sub_08120264(manager);
    if (!e)
        return;
    Memory_Clear(e->block5BC, 16);
    e->field8AA = 0xFF;
    sub_081200C8(e);
    anim = e->anim;
    sub_0811C9F8(e, anim, 0x3D99);
    sub_0811C89C(e->block68, anim, e->field4B);
    sub_08112BA8(e->block10);
    sub_0811FDB4(e);
    {
        void (*update)(void) = sub_08120B44;
        u8 *active = &e->active;
        u32 zero = 0;
        one = 1;
        *active = one;
        e->update = update;
        e->field84C = zero;
    }
    sub_081202CC(e);
    {
        struct DrawFlags *draw = &e->draw;
        draw->flags = one | draw->flags;
    }
    sub_0811EDC0(e->blockB0, e->block618, 0, 0);
    e->field8B4 = one;
}
