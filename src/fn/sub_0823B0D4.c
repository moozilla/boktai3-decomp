#include "global.h"
#include "scene_actor_terrain.h"

#ifdef NONMATCHING
// fuzzy: 99.5%
// foundry draft (worker best 99.5%); what differs is mostly register allocation and instruction order
struct Pos {
    u16 x, h, z;
};

struct Obj {
    u32 unk0;
    u8 pad4[0x14];
    u32 idx;
    u8 pad1C[8];
    u32 val24;
    u8 pad28[0x25C];
    u8 b284[0x10];
    u8 pad294[0x4];
    u8 pad298[0x50];
    u8 b2E8[0x130];
    u8 b418;
    u8 b419;
    u8 pad41A[0x126];
    struct Pos pos;
};

void sub_0821BC68(u32, u8 *);
u32 SceneBody_SetMovementProbe(u8 *p, u32 a, u16 b, u16 c);
void SceneBody_SetKeyframeDisplay(void *, void *);
u32 SceneBody_SetPositionOffset(u8 *p, u32 *q);
void SceneBody_Init(void *, u32, void *, s32, s32, void *);
void SceneActorContacts_RegisterNode();
void sub_0823AE5C();
void sub_0823AF44();

u32 sub_0823B0D4(struct Obj *p, u32 arg)
{
    s32 x, y, t, h, kind;
    u8 *cell;
    u8 *r5;
    u32 q[2];
    u32 m, v;
    u16 *tbl;
    struct Pos *pos;
    struct Pos *pos2;

    if (Script_SeekToKeyword(0x70)) {
        *(u16 *)((u8 *)p + 0x540) = Script_GetValue();
        *(u16 *)((u8 *)p + 0x542) = Script_GetValue();
        *(u16 *)((u8 *)p + 0x544) = Script_GetValue();
    } else {
        *(u16 *)((u8 *)p + 0x540) = 0;
        *(u16 *)((u8 *)p + 0x542) = 0;
        *(u16 *)((u8 *)p + 0x544) = 0;
    }
    pos = &p->pos;
    x = (s16)pos->x >> 8;
    y = (s16)pos->z >> 8;
    if (x < 0 || y < 0 || (u32)x >= gUnk_030052F8 || (u32)y >= gUnk_030052FC)
        t = 0;
    else
        t = gUnk_030052F4->rows[y] + x;
    cell = SceneActorTerrain_GetCell(t);
    kind = *cell >> 4;
    h = (*cell & 15) << 8;
    switch (kind) {
    case 1:
        h -= *(u8 *)&pos->z;
        break;
    case 2:
        h -= *(u8 *)&pos->x;
        break;
    }
    p->pos.h = h;
    t = 0;
    r5 = (u8 *)p + 0x24;
    pos2 = &p->pos;
    SceneBody_Init(r5, arg, pos2, 0, *(u8 *)((u8 *)p + 0x18), p);
    sub_0823AE5C(p);
    sub_0823AF44(p);
    p->b419 = t;
    SceneBody_SetKeyframeDisplay(r5, (u8 *)p + 0x8c);
    sub_0821BC68((u32)p->b284, (u8 *)pos2);
    SceneBody_SetMovementProbe(r5, (u32)p->b284, 30, 30);
    m = 0xFFFF0000;
    q[0] = 0x12C0000;
    q[1] &= m;
    SceneBody_SetPositionOffset(r5, q);
    v = p->val24;
    *(u16 *)((u8 *)p + 0x294) = v;
    SceneActorContacts_RegisterNode(p->b2E8, r5, 2);
    tbl = (u16 *)0x02000508;
    tbl[p->idx] = p->val24;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0823B0D4);
#endif
