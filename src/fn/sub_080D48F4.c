#include "global.h"
void SceneActorAnimation_UpdateDirectionalEntry(void);
void SceneActorAttachment_ReplaceResponseSlot(u8 *, u32, u32, u32);
void SceneActorBehavior_CallActionScript(u8 *);
void Mummy_EnterIdleControl(u8 *);
inline static u8 TakeFlag(u8 *p)
{
    if (*p) {
        *p = 0;
        return 1;
    }
    return 0;
}

struct S {
    u8 pad0[0x59];
    u8 at59;
    u8 pad1[0x98 - 0x5A];
    u8 at98;
    u8 pad2[0xE9 - 0x99];
    u8 atE9;
    u8 pad3[0x104 - 0xEA];
    u32 at104;
    u8 pad4[0x114 - 0x108];
    u32 flags114;
    u8 pad5[0x21C - 0x118];
    void (*callback)(void);
    u8 pad6[0x2AC - 0x220];
    u32 counter;
    u8 done;
    u8 queued;
    u8 taken;
    u8 pad7[2];
    u8 code;
    u8 pad8[0x2C8 - 0x2B6];
    u16 at2C8;
    u8 pad9[0x2D4 - 0x2CA];
    u16 flags2D4;
    u8 pad10[0x414 - 0x2D6];
    u16 flags414;
    u16 at416;
    u16 at418;
};
#define Mummy_UpdateScriptedAction sub_080D48F4
void sub_080D48F4(struct S *obj)
{
    if (TakeFlag(((u8 *)obj) + 0x2B1)) {
        void (*fn)(void) = SceneActorAnimation_UpdateDirectionalEntry;
        u8 code = 10;
        u32 zero;
        u32 *f;
        s32 mask;
        u16 *h;
        u16 value;
        s32 off;
        {
            u8 *taken = &obj->taken;
            zero = 0;
            *taken = 1;
        }
        do {
            u32 field = 0x2B0;
            do {
                ((u8 *)obj)[field] = zero;
            } while (0);
            field += 5;
            ((u8 *)obj)[off = field] = code;
            obj->callback = fn;
            field += 0x13;
            *((u16 *)(((u8 *)obj) + field)) = zero;
        } while (0);
        f = (u32 *)(((u8 *)obj) + 0x114);
        mask = -2;
        *f &= mask;
        *((u32 *)(((u8 *)obj) + 0x104)) = zero;
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
    } else if (obj->done) {
        Mummy_EnterIdleControl((u8 *)obj);
    } else {
        ++(*((u32 *)(((u8 *)obj) + 0x2AC)));
    }
}
