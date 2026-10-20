#include "global.h"

void SceneActorAttachment_ReplaceResponseSlot(u8 *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(u8 *);

struct S {
    u8 pad0[0x59];
    u8 at59;
    u8 pad1[0x98 - 0x5A];
    u8 at98;
    u8 pad2[0xE9 - 0x99];
    u8 atE9;
    u8 pad3[0x2D4 - 0xEA];
    u16 flags2D4;
    u8 pad4[0x3D0 - 0x2D6];
    u8 *owner;
    u8 pad5[0x414 - 0x3D4];
    u16 flags414;
    u16 at416;
    u16 at418;
};

void sub_080E8158(struct S *obj)
{
    u16 *h;
    u16 value;
    s32 off;
    s32 mask;
    u16 *p;
    {
        u32 byte = obj->atE9;
        obj->at59 = byte;
    }
    SceneActorAttachment_ReplaceResponseSlot((u8 *)obj, 0, 0, obj->at98);
    SceneActorBehavior_CallActionScript((u8 *)obj);
    h = (u16 *)(((u8 *)obj) + 0x414);
    {
        u32 flag = 4;
        flag |= *h;
        *h = flag;
    }
    {
        u32 flag = 0x80;
        u16 *flags = (u16 *)(((u8 *)obj) + 0x2D4);
        flag |= *flags;
        *flags = flag;
    }
    off = 0x416;
    value = *((u16 *)(((u8 *)obj) + off));
    off += 2;
    *((u16 *)(((u8 *)obj) - (-off))) = value;
    p = (u16 *)(obj->owner + 0x65C);
    mask = -2;
    mask &= *p;
    *p = mask;
}
