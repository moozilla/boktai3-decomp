#include "global.h"
#include "link/handshake/slot_monitor.h"

s32 Serial_ReceivePacket(void *);
s32 SerialLink_QueueSendWord(u32);
void SerialLink_StartTransferIfIdle(void);
void Serial_StopCommunication(void);

void sub_08108C94(struct SlotMonitor *obj)
{
    s32 i;
    s32 count = 0;
    s32 matches = 0;
    s32 ready = 0;
    s32 contiguous = TRUE;

    obj->receiveResult = Serial_ReceivePacket(obj->slots);
    obj->sendResult = SerialLink_QueueSendWord(obj->flags);
    if (!obj->finished && obj->result == 0)
        SerialLink_StartTransferIfIdle();
    if (SlotMonitor_CheckTimeout(obj)) {
        obj->succeeded = FALSE;
        obj->finished = TRUE;
        obj->f20 = 0;
        return;
    }
    if (obj->receiveResult >= 0) {
        obj->ticks58++;
        for (i = 0; i < 4; i++) {
            if (obj->slots[i] != 0xFFFF) {
                if (contiguous) {
                    if (obj->slots[i] == (0x1BC0 | obj->config)) {
                        matches++;
                    } else if ((obj->slots[i] & 0xFFC0) == 0x2FC0) {
                        obj->mode = obj->slots[0] & 0x3F;
                        if (obj->mode) {
                            matches++;
                            ready++;
                        }
                    }
                }
                count++;
            } else {
                contiguous = FALSE;
            }
        }
        if (count == 2 && matches == 2) {
            obj->readyFrames++;
            if (obj->readyFrames >= obj->limit)
                obj->flags = obj->mode | 0x2FC0;
            obj->elapsed = 0;
        } else {
            if (LinkHandshake_AdvanceTimeout(obj))
                obj->finished = TRUE;
            obj->readyFrames = 0;
        }
        if (ready == 2) {
            obj->succeeded = TRUE;
            obj->finished = TRUE;
        }
        obj->errorFrames = 0;
    } else {
        obj->flags = 0x1BC0 | obj->config;
        if (obj->receiveResult == -3) {
            obj->field4c++;
            if (obj->errorFrames > 29) {
                Serial_StopCommunication();
                obj->callback = LinkHandshake_UpdateRestartState;
                obj->restartPending = TRUE;
                return;
            }
            obj->errorFrames++;
            if (LinkHandshake_AdvanceTimeout(obj))
                obj->finished = TRUE;
        } else {
            obj->readyFrames = 0;
            if (obj->retryTimer > 29) {
                Serial_StopCommunication();
                obj->callback = LinkHandshake_UpdateRestartState;
                obj->restartPending = TRUE;
                return;
            }
            obj->retryTimer++;
            if (LinkHandshake_AdvanceTimeout(obj))
                obj->finished = TRUE;
            obj->errorFrames = 0;
        }
    }
}
