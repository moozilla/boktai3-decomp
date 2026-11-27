#include "engine/engine.h"
#include "link/wireless/wireless_trade_menu.h"

int WirelessTradeMenu_Update(struct WirelessGameExchange *);
u32 WirelessTradeMenu_Destroy(struct WirelessGameExchange *);
int WirelessTradeMenu_InitFromScript(struct WirelessGameExchange *);

struct WirelessGameExchange *WirelessTradeMenu_Create(void)
{
    struct WirelessGameExchange *menu = Task_Create(2, sizeof(*menu));
    if (menu) {
        Task_SetCallbacks(menu, (void (*)(void *))WirelessTradeMenu_Update,
                          (void (*)(void *))WirelessTradeMenu_Destroy);
        if (WirelessTradeMenu_InitFromScript(menu) < 0) {
            Task_Kill(menu);
            return 0;
        }
        menu->task.unk16 = 1;
    }
    return menu;
}
