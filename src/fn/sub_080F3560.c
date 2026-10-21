#include "global.h"
// foundry draft (worker best 98.8%); what differs is mostly register allocation and instruction order
struct ChildGroup {
    u8 pad0[0x7E];
    s16 field7E;
    u8 pad80[0xB04];
    s32 position[3];
    u8 padB90[0xE0];
    u8 display[0xEC];
    s16 fieldD5C;
    u8 fieldD5E;
    u8 padD5F[0x19];
    u16 damage;
    u8 padD7A[2];
    u16 fieldD7C;
    u8 padD7E[8];
    s16 fieldD86;
};
void SceneHud_SetTimedState(u32, s32, s32, u32, u32, s32);
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
struct ChildGroupOffset {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
static inline void CpuSetC(void *dst, u32 control, void *src)
{
    CpuSet(src, dst, control);
}
void ChildGroup_ShowDamageNumber(struct ChildGroup *p, u32 flags, s32 mode)
{
    struct {
        u32 fill;
        struct ChildGroupOffset offset;
    } scratch;
    struct ChildGroupOffset *vector;
    SceneHud_SetTimedState(p->fieldD7C, p->field7E, p->fieldD5C, p->fieldD5E, (flags >> 2) & 1,
                           p->fieldD86);
    vector = &scratch.offset;
    /* fill source is initialized with the call argument. */
    scratch.fill = 0;
    CpuSetC(vector, 0x05000002, &scratch.fill);
    vector->y = -120;
    DamageNumberDisplay_Show(p->display, p->damage, mode, p->position, vector, 1);
    p->damage = 0;
}
