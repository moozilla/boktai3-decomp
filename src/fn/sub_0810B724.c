#include "global.h"
#include "engine.h"
#include "render.h"
#include "ram.h"

struct Reg1745Run {
    u8 pad0[0x1c];
    u8 state;
    u8 field1D;
    u8 field1E;
    u8 field1F;
    u8 pad20[0x10];
    s32 counter30;
    u8 pad34[0xA];
    u16 field3E;
    u32 field40;
    s32 field44;
    s32 field48;
};
struct Reg1745Hdr {
    u8 pad0[0x18];
    s16 field18;
};

extern u32 gUnk_03005408, gUnk_030053FC;
extern struct Reg1745Hdr *gUnk_020004B4;
s32 Serial_ReceivePacket(u32);
s32 SerialLink_QueueSendWord(u32);
void SerialLink_StartTransferIfIdle(void);
void Registry1745_BeginShutdown(void *, void (*)(void));
void ScriptPanel_OpenTextWindow(void);
void System_ShutdownAndSoftReset(void);

static inline void FlagSet(u32 m)
{
    gEnginePauseFlags |= m;
}
static inline void FlagClear(u32 m)
{
    gEnginePauseFlags &= ~m;
}

u32 Registry1745_RunSession(struct Reg1745Run *p)
{
    struct Reg1745Run *q = p;
    gLinkSessionStep = 0;
    if (p->state > 4)
        gUnk_03005408 = 1;
    gLinkSessionStep = 1;
    if (p->state > 2) {
        u32 st;
        p->field48 = Serial_ReceivePacket(p->field40);
        gLinkSessionStep = 2;
        p->field44 = SerialLink_QueueSendWord(p->field3E);
        gLinkSessionStep = 3;
        st = p->state;
        if (st > 4) {
            if (gUnk_020004B4 && gUnk_020004B4->field18 == 0 && st != 0x19 && st != 0x1b)
                SerialLink_StartTransferIfIdle();
        } else if (p->field1D == 0 && st != 0x19 && st != 0x1b)
            SerialLink_StartTransferIfIdle();
        gLinkSessionStep = 4;
    }
    gLinkSessionStep = 5;
    {
        u32 st = q->state;
        if (st > 4 && st != 0x19 && st != 0x1b && st != 0x1c) {
            gLinkSessionStep = 6;
            if (q->field48 == -3) {
                if (st != 0xb)
                    FlagSet(1);
                gLinkSessionStep = 8;
                if (q->field1F <= 0xfe) {
                    q->field1F++;
                    if (st > 4 && q->field1F > 0xe && gUnk_030053FC == 0)
                        Registry1745_BeginShutdown(q, ScriptPanel_OpenTextWindow);
                }
                gLinkSessionStep = 9;
                return 0;
            }
            FlagClear(1);
            q->field1F = 0;
            gLinkSessionStep = 10;
        }
    }
    gLinkSessionStep = 12;
    Callback_InvokeActor(p, *(void **)((u32)q + 0xb0));
    q->counter30++;
    gLinkSessionStep = 14;
    if (!({
            s32 done;
            if (p->field1E) {
                Task_Kill(p);
                if (p->state == 0) {
                    gLinkSessionStep = 15;
                    System_ShutdownAndSoftReset();
                } else {
                    u32 cb = *(u32 *)((u32)p + 0xcc);
                    if (cb) {
                        gLinkSessionStep = 16;
                        Script_Call(cb, 0);
                    } else {
                        gLinkSessionStep = 17;
                        System_ShutdownAndSoftReset();
                    }
                }
                done = 1;
            } else
                done = 0;
            asm("" : "+r"(done));
            done;
        }))
        gLinkSessionStep = 18;
    return 0;
}
