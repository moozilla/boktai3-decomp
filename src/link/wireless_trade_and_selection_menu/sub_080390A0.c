#include "link/wireless_trade_and_selection_menu/wireless_trade_menu.h"
#include "engine/shared/ram.h"

void TextWindow_Destroy(u32);
void Bg_ClearTilemap(u32);

u32 WirelessTradeMenu_Destroy(struct WirelessGameExchange *menu)
{
    TextWindow_Destroy(menu->window);
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(2);
    return gWirelessTradeMenu = 0;
}
