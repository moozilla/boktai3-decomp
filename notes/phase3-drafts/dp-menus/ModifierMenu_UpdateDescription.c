#include "modifier_list.h"
u32 StateFlags_GetCategoryBit(s32, s32);
void ScrollingDialogue_SetScript(const u8 *);
extern const u8 gUnk_086058C8[];
void ModifierMenu_UpdateDescription(struct ModifierMenu *menu)
{
    const u8 *text = 0;
    s32 categoryValue;
    s32 itemValue;
    const u8 **sourceLocation;
    switch (menu->state) {
        case 0: {
            const u8 *source = menu->textSources[0];
            s32 index = menu->mode;
            text = Text_LookupString(Script_ParseStringRef(source) + index);
            break;
        }
        case 1:
            if (menu->mode == 0) {
                s32 index;
                const u8 *source;
                s8 *category = &menu->choice.cursor.category;
                s8 *item;
                s32 categoryArg = *category;
                s32 tableIndex;
                const u8 **table;
                item = &menu->choice.cursor.index;
                if (!StateFlags_GetCategoryBit(categoryArg, *item))
                    goto unavailable;
                tableIndex = *category + 7;
                table = &menu->textSources[tableIndex];
                sourceLocation = table;
                index = *item;
                source = *sourceLocation;
                text = Text_LookupString(Script_ParseStringRef(source) + index);
                break;
            }
            if (menu->mode == 1) {
                s8 *category = &menu->choice.cursor.category;
                s8 *item;
                s32 categoryArg = *category;
                s32 itemArg;
                item = &menu->choice.cursor.index;
                itemArg = *item;
                if (StateFlags_GetCategoryBit(4, categoryArg * 3 * 2 + itemArg)) {
                    categoryValue = *category;
                    itemValue = *item;
                    sourceLocation = &menu->textSources[11];
                    goto groupLookup;
                }
            unavailable:
                text = Text_LookupString(Script_ParseStringRef(menu->textSources[0]) + 6);
                break;
            } else {
                s8 *category = &menu->choice.cursor.category;
                s8 *item;
                s32 categoryArg = *category;
                s32 itemArg;
                item = &menu->choice.cursor.index;
                itemArg = *item;
                if (StateFlags_GetCategoryBit(5, categoryArg * 3 * 2 + itemArg)) {
                    categoryValue = *category;
                    itemValue = *item;
                    sourceLocation = &menu->textSources[12];
                groupLookup: {
                    const u8 *source = *sourceLocation;
                    s32 index = categoryValue * 3 * 2 + itemValue;
                    text = Text_LookupString(Script_ParseStringRef(source) + index);
                }
                } else
                    text = Text_LookupString(Script_ParseStringRef(menu->textSources[0]) + 6);
            }
            break;
        case 2:
        default:
            break;
    }
    if (text)
        ScrollingDialogue_SetScript(text);
    else
        ScrollingDialogue_SetScript(gUnk_086058C8);
}
