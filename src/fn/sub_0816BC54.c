#include "global.h"

#ifdef NONMATCHING
// fuzzy: 72.0%
// Open: retained link pointers, flag masks and branch tail sharing still differ.
// confidence=low: refreshes map navigation arrows and their destination indices.
#define MapMenu_RefreshNavigationArrows sub_0816BC54
struct NavigationArrow {
    u32 flags;
    u8 unknown4[0x5C];
};
struct NavigationPage {
    u8 map;
    u8 unknown1[5];
    s8 neighbors[4];
    u8 solidLinks;
    s8 alternate[4];
    u8 unknownF;
};
struct NavigationMenu {
    u8 unknown0[0x3A68];
    struct NavigationArrow arrows[4];
    u8 unknown3BE8[0x48F8 - 0x3BE8];
    struct NavigationPage *pages;
    u8 unknown48FC[8];
    u8 page, showAll;
    u8 unknown4906[2];
    s8 destinations[4];
    u8 unknown490C[0x10];
    u16 currentPage;
};
u32 GameState_GetMapRevealFlag(u32);
static inline void RefreshLink(struct NavigationMenu *menu, struct NavigationPage *page,
                               s32 direction, struct NavigationPage **pages, u8 *selectedPage)
{
    if (page->neighbors[direction] != -1) {
        if (GameState_GetMapRevealFlag((*pages)[page->neighbors[direction]].map)) {
            s8 *destination = &menu->destinations[direction];
            struct NavigationArrow *arrow = &menu->arrows[direction];
            *destination = page->neighbors[direction];
            arrow->flags &= ~1;
            if ((page->solidLinks & (u32)(1 << direction)) != 0)
                arrow->flags &= ~0x200;
            else
                arrow->flags |= 0x200;
        } else if (menu->page != menu->currentPage) {
            if ((*pages)[page->neighbors[direction]].map == (*pages)[menu->currentPage].map) {
                s8 *destination = &menu->destinations[direction];
                struct NavigationArrow *arrow = &menu->arrows[direction];
                *destination = menu->currentPage;
                arrow->flags &= ~1;
                if (page->neighbors[direction] == menu->currentPage)
                    arrow->flags &= ~0x200;
                else
                    arrow->flags |= 0x200;
            }
        }
    }
}
static inline void RefreshAlternate(struct NavigationMenu *menu, struct NavigationPage *page,
                                    s32 direction, struct NavigationArrow *arrow, s8 *destination,
                                    struct NavigationPage **pages, u8 *selectedPage)
{
    if (GameState_GetMapRevealFlag((*pages)[page->alternate[direction]].map)) {
        *destination = page->alternate[direction];
        arrow->flags &= ~1;
        if ((page->solidLinks & (1 << direction)) &&
            page->neighbors[direction] == page->alternate[direction])
            arrow->flags &= ~0x200;
        else
            menu->arrows[direction].flags |= 0x200;
    }
}
void MapMenu_RefreshNavigationArrows(struct NavigationMenu *menu)
{
    struct NavigationArrow *second;
    struct NavigationArrow *first;
    struct NavigationArrow *third;
    struct NavigationArrow *fourth;
    u8 *destination0;
    s8 *destination1, *destination2, *destination3;
    struct NavigationPage *page;
    struct NavigationPage **pages;
    u8 *selectedPage;
    first = (struct NavigationArrow *)((u8 *)menu + 0x3a68);
    first->flags |= 1;
    second = (struct NavigationArrow *)((u8 *)menu + 0x3ac8);
    second->flags |= 1;
    third = (struct NavigationArrow *)((u8 *)menu + 0x3b28);
    third->flags |= 1;
    fourth = (struct NavigationArrow *)((u8 *)menu + 0x3b88);
    fourth->flags |= 1;
    destination0 = &menu->destinations[0];
    *destination0 = -1;
    destination1 = &menu->destinations[1];
    *destination1 = -1;
    destination2 = &menu->destinations[2];
    *destination2 = -1;
    destination3 = &menu->destinations[3];
    *destination3 = -1;
    pages = &menu->pages;
    selectedPage = &menu->page;
    page = &(*pages)[*selectedPage];
    if (menu->showAll) {
        RefreshLink(menu, page, 0, pages, selectedPage);
        RefreshLink(menu, page, 1, pages, selectedPage);
        RefreshLink(menu, page, 2, pages, selectedPage);
        RefreshLink(menu, page, 3, pages, selectedPage);
    } else {
        if (page->alternate[0] != -1)
            RefreshAlternate(menu, page, 0, first, destination0, pages, selectedPage);
        else if (page->alternate[1] != -1)
            RefreshAlternate(menu, page, 1, second, destination1, pages, selectedPage);
        else if (page->alternate[2] != -1)
            RefreshAlternate(menu, page, 2, third, destination2, pages, selectedPage);
        else if (page->alternate[3] != -1)
            RefreshAlternate(menu, page, 3, fourth, destination3, pages, selectedPage);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0816BC54);
#endif
