#include "global.h"
// 0xFFFF lives in an unallocated pseudo outside the loop, so reload
// rematerialises it into r7 at the compare.

struct SlotMonitor {
    u8 unk0[0x20];
    s32 field20;
    s32 selection;
    s32 field28;
    s32 finished;
    u8 unk30[4];
    s32 field34, field38, field3c;
    u8 unk40[4];
    s32 field44, result, field4c, retryTimer;
    u8 unk54[4];
    s32 ticks;
    u8 unk5c[4];
    u16 flags;
    u16 peers[4];
    u16 limit;
    u16 config;
    u8 field6e;
    u8 field6f;
    u8 unk70[0xC];
    void (*callback)(void);
};

s32 Serial_ReceivePacket(void *);
s32 SerialLink_QueueSendWord(u32);
void SerialLink_StartTransferIfIdle(void);
void Serial_StopCommunication(void);
u8 SlotMonitor_CheckTimeout(struct SlotMonitor *);
u8 LinkHandshake_AdvanceTimeout(void *);
void LinkHandshake_UpdateRestartState(void);

static inline u16 SlotMonitor_Peer(u16 *base, s32 i)
{
    return base[i];
}
static inline u16 SlotMonitor_Flags(struct SlotMonitor *obj)
{
    return obj->flags;
}
void sub_08108C94(struct SlotMonitor *obj)
{
    s32 i;
    s32 count = 0;
    s32 matches = 0;
    s32 ready = 0;
    s32 first = 1;
    s32 stop;

    obj->result = Serial_ReceivePacket(obj->peers);
    obj->field44 = SerialLink_QueueSendWord(SlotMonitor_Flags(obj));
    if (obj->finished == 0 && obj->selection == 0)
        SerialLink_StartTransferIfIdle();
    stop = (u8)SlotMonitor_CheckTimeout(obj);
    if (stop) {
        obj->field28 = 0;
        obj->finished = 1;
        obj->field20 = 0;
        return;
    }
    if (obj->result >= 0) {
        u16 *base;
        u16 *p;
        s32 none;
        obj->ticks++;
        base = obj->peers;
        none = 0xFFFF;
        for (i = 0; i < 4; i++) {
            u16 v = SlotMonitor_Peer(base, i);
            if (v != none) {
                if ((u8)first) {
                    if (v == (0x1BC0 | obj->config)) {
                        matches++;
                    } else if ((SlotMonitor_Peer(base, i) & 0xFFC0) == 0x2FC0) {
                        obj->field6f = *(u8 *)base & 0x3F;
                        if (obj->field6f) {
                            matches++;
                            ready++;
                        }
                    }
                }
                count++;
            } else {
                first = 0;
            }
        }
        if (count == 2 && matches == 2) {
            obj->field38++;
            if (obj->field38 >= obj->limit)
                obj->flags = obj->field6f | 0x2FC0;
            obj->field34 = 0;
        } else {
            if (LinkHandshake_AdvanceTimeout(obj))
                obj->finished = 1;
            obj->field38 = 0;
        }
        if (ready == 2) {
            obj->field28 = 1;
            obj->finished = 1;
        }
        obj->field3c = 0;
    } else {
        obj->flags = 0x1BC0 | obj->config;
        if (obj->result == -3) {
            obj->field4c++;
            if (obj->field3c > 29)
                goto timeout;
            obj->field3c++;
            if (LinkHandshake_AdvanceTimeout(obj))
                obj->finished = 1;
        } else {
            obj->field38 = stop;
            if (obj->retryTimer > 29) {
            timeout:
                Serial_StopCommunication();
                obj->callback = LinkHandshake_UpdateRestartState;
                obj->field6e = 1;
                return;
            }
            obj->retryTimer++;
            if (LinkHandshake_AdvanceTimeout(obj))
                obj->finished = 1;
            obj->field3c = stop;
        }
    }
}
