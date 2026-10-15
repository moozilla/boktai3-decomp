#include "cell_gate.h"
#define CellGate_UpdateForcedDirection sub_08050404
static inline u8 specialMode(void)
{
    u32 mode = gUnk_02000534;
    u32 result = 0;
    if (mode && gUnk_02000500 == 5)
        result = 1;
    return result;
}
void CellGate_UpdateForcedDirection(struct CellGate *gate)
{
    s32 result = sub_08050188(gate);
    if (result) {
        result = sub_08050128(gate);
        if (result) {
            if (!gate->playerInside) {
                gUnk_02000580->gateCount++;
                if (gate->flags & 8)
                    gUnk_02000580->direction = 7;
                else
                    gUnk_02000580->direction = 1;
                gate->playerInside = 1;
            }
            if (gate->registered) {
                sub_0821D6D0(gate->registration);
                gate->registered = 0;
            }
            sub_080503A0(gate, 40);
            if (gUnk_02000580->transitionMode != 4 && !specialMode()) {
                struct CellGatePlayer *player = gUnk_02000580;
                u32 action = player->actionMode;
                if (action == 3)
                    goto done;
                if (action == 4)
                    goto done;
                {
                    if (gate->flags & 8)
                        sub_08158DF0(player, 7);
                    else
                        sub_08158DF0(player, 1);
                }
            }
        } else {
            if (gate->playerInside) {
                gUnk_02000580->gateCount--;
                gate->playerInside = result;
            }
            if (!sub_08050310(gate)) {
                if (!gate->registered) {
                    sub_0821D698(gate->registration, gate->cell, 0, gate->height, 255, 0x80B);
                    gate->registered = 1;
                }
            } else
                goto remove;
        }
    } else {
        if (gate->playerInside) {
            gUnk_02000580->gateCount--;
            gate->playerInside = result;
        }
    remove:
        if (gate->registered) {
            sub_0821D6D0(gate->registration);
            gate->registered = result;
        }
    }
done:
    return;
}
