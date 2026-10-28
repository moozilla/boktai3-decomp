#include "wireless_link_indicator.h"
void WirelessLinkIndicator_UpdateLinkStatus(struct WirelessLinkIndicator *input)
{
    struct WirelessLinkIndicator *indicator = input;
    DEBUG_NOP();
    if (!Wireless_GetStatus()) {
        s32 strength = 255;
        if (Wireless_IsSessionConnected()) {
            struct RfuLinkStatus *status = gRfuLinkStatus;
            if (!status->parentChild) {
                s32 slot = Wireless_GetConnectedParentSlot();
                strength = gRfuLinkStatus->strength[slot];
                WirelessLinkIndicator_SelectStrengthFrame(indicator, strength);
            } else {
                s32 index = 0;
                s32 slots;
                u8 *values;
                u32 connected;
                slots = status->connSlotFlag;
                connected = 1;
                values = status->strength;
                do {
                    if ((slots >> index) & connected) {
                        s32 current = values[index];
                        if (strength > current) {
                            strength = current;
                        }
                    }
                    ++index;
                } while (index <= 3);
                if (!gRfuLinkStatus->connSlotFlag) {
                    WirelessLinkIndicator_SelectStrengthFrame(indicator, 0);
                } else {
                    WirelessLinkIndicator_SelectStrengthFrame(indicator, strength);
                }
            }
            indicator->animation.flags &= ~1;
        } else {
            indicator->animation.flags |= 1;
        }
    }
}
