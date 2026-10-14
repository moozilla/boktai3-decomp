// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#define ModifierMenu_UpdateDescription sub_08041020
u32 sub_0811DD94(s32, s32);
void sub_0803343C(const u8 *);
extern const u8 gUnk_086058C8[];
void ModifierMenu_UpdateDescription(struct ModifierMenu *menu)
{
    const u8 *text = 0;
    const u8 *source;
    register s32 index asm("r4");
    register s32 categoryValue asm("r1");
    s32 itemValue;
    const u8 **sourceLocation;
    switch (menu->state) {
        case 0:
            source = menu->textSources[0];
            index = menu->mode;
            goto lookup;
        case 1:
            if (menu->mode == 0) {
                s8 *category = &menu->category;
                s8 *item;
                s32 categoryArg = *category;
                s32 tableIndex;
                register const u8 **table asm("r1");
                item = &menu->index;
                if (!sub_0811DD94(categoryArg, *item))
                    goto unavailable;
                tableIndex = *category + 7;
                tableIndex *= sizeof(*table);
                table = menu->textSources;
                table = (const u8 **)((u8 *)table + tableIndex);
                asm("" : "+r"(table));
                sourceLocation = table;
                index = *item;
                source = *sourceLocation;
                goto lookup;
            }
            if (menu->mode == 1) {
                s8 *category = &menu->category;
                s8 *item;
                s32 categoryArg = *category;
                s32 itemArg;
                asm("" : "+r"(categoryArg));
                item = &menu->index;
                itemArg = *item;
                if (sub_0811DD94(4, categoryArg * 3 * 2 + itemArg)) {
                    categoryValue = *category;
                    itemValue = *item;
                    sourceLocation = &menu->textSources[11];
                    goto groupLookup;
                }
            unavailable:
                text = Text_LookupString(Script_ParseStringRef(menu->textSources[0]) + 6);
                break;
            } else {
                s8 *category = &menu->category;
                s8 *item;
                s32 categoryArg = *category;
                s32 itemArg;
                asm("" : "+r"(categoryArg));
                item = &menu->index;
                itemArg = *item;
                if (sub_0811DD94(5, categoryArg * 3 * 2 + itemArg)) {
                    categoryValue = *category;
                    itemValue = *item;
                    sourceLocation = &menu->textSources[12];
                groupLookup:
                    source = *sourceLocation;
                    index = categoryValue * 3 * 2 + itemValue;
                lookup:
                    text = Text_LookupString(Script_ParseStringRef(source) + index);
                } else
                    text = Text_LookupString(Script_ParseStringRef(menu->textSources[0]) + 6);
            }
            break;
        case 2:
        default:
            break;
    }
    if (text)
        sub_0803343C(text);
    else
        sub_0803343C(gUnk_086058C8);
}
