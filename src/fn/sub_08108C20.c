#include "global.h"
#define SlotMonitor_AdvanceTimeout LinkHandshake_AdvanceTimeout
struct SlotMonitor {
    u8 unk0[0x24];
    s32 selection;
    u8 unk28[4];
    s32 finished;
    u8 unk30[8];
    s32 field38, field3c, retryTimer;
    u8 unk44[0x10];
    s32 ticks;
    u8 unk58[8];
    u16 flags;
    u8 unk62[10];
    u16 config;
    u8 unk6e[14];
    void (*callback)(void);
};
s32 Serial_DetectRoleAndEnableInterrupt(void);
void SerialLink_ResetMultiplayer(void);
u8 LinkHandshake_AdvanceTimeout(void *);
void sub_08108C94(void);
void SlotMonitor_PollResult(struct SlotMonitor *obj)
{
    obj->ticks++;
    obj->selection = Serial_DetectRoleAndEnableInterrupt();
    if (obj->selection < 0) {
        if (obj->retryTimer > 29) {
            SerialLink_ResetMultiplayer();
            obj->selection = Serial_DetectRoleAndEnableInterrupt();
            obj->retryTimer = 0;
        } else
            obj->retryTimer++;
    } else {
        obj->callback = sub_08108C94;
        obj->flags = 0x1bc0 | obj->config;
        obj->field3c = 0;
        obj->field38 = 0;
        obj->retryTimer = 0;
        return;
    }
    if (LinkHandshake_AdvanceTimeout(obj))
        obj->finished = 1;
    obj->field3c = 0;
    obj->field38 = 0;
}
