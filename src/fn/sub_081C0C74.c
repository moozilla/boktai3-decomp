#include "global.h"
extern const u8 gUnk_0824F934[];
extern const u8 gUnk_0824F940[];
extern const u8 gUnk_0824F948[];
extern const u8 gUnk_0824F954[];
void Menu_EraseRect(u32, u32, u32, u32);
void Menu_DrawText(u32, u32, const u8 *);
void sub_081C0C74(void)
{
    Menu_EraseRect(2, 4, 10, 1);
    Menu_DrawText(2, 4, gUnk_0824F934);
    Menu_EraseRect(2, 7, 10, 1);
    Menu_DrawText(2, 7, gUnk_0824F940);
    Menu_EraseRect(2, 10, 10, 1);
    Menu_DrawText(2, 10, gUnk_0824F948);
    Menu_EraseRect(2, 13, 10, 1);
    Menu_DrawText(2, 13, gUnk_0824F954);
}
