#include "global.h"
#include "engine.h"
#include "render.h"
#include "ram.h"
#include "runtime.h"
#include "area_effect_link.h"

struct Reg1745Run {
    u8 pad0[0x1c];
    u8 state;
    u8 field1D;
    u8 shutdownRequested;
    u8 field1F;
    u8 pad20[0x10];
    s32 counter30;
    u8 pad34[0xA];
    u16 field3E;
    u32 field40;
    s32 field44;
    s32 field48;
    u8 pad4C[0xB0 - 0x4C];
    void *callback;
    u8 padB4[0xCC - 0xB4];
    u32 shutdownScript;
};
struct Reg1745Hdr {
    u8 pad0[0x18];
    s16 field18;
};

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

static inline u8 Registry1745_FinishIfRequested(struct Reg1745Run *p)
{
    gLinkSessionStep = 14;
    if (p->shutdownRequested) {
        Task_Kill(p);
        if (p->state == 0) {
            gLinkSessionStep = 15;
            System_ShutdownAndSoftReset();
        } else {
            u32 cb = p->shutdownScript;
            if (cb) {
                gLinkSessionStep = 16;
                Script_Call(cb, 0);
            } else {
                gLinkSessionStep = 17;
                System_ShutdownAndSoftReset();
            }
        }
        return 1;
    }
    return 0;
}

u32 Registry1745_RunSession(struct Reg1745Run *p)
{
    struct Reg1745Run *q = p;
    gLinkSessionStep = 0;
    if (p->state > 4)
        gSceneDialogActive = 1;
    gLinkSessionStep = 1;
    if (p->state > 2) {
        u32 st;
        p->field48 = Serial_ReceivePacket(p->field40);
        gLinkSessionStep = 2;
        p->field44 = SerialLink_QueueSendWord(p->field3E);
        gLinkSessionStep = 3;
        st = p->state;
        if (st > 4) {
            if ((*(struct Reg1745Hdr **)&gLinkSession) &&
                (*(struct Reg1745Hdr **)&gLinkSession)->field18 == 0 && st != 0x19 && st != 0x1b)
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
                    if (st > 4 && q->field1F > 0xe && gSceneTransitionActive == 0)
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
    Callback_InvokeActor(p, q->callback);
    q->counter30++;
    if (!Registry1745_FinishIfRequested(p))
        gLinkSessionStep = 18;
    return 0;
}
