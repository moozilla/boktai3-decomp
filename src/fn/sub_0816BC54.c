#include "global.h"

// MATCHING_CONCESSION: register-pinning

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
void MapMenu_RefreshNavigationArrows(struct NavigationMenu *menu)
{
    struct NavigationArrow *second;
    struct NavigationArrow *first;
    struct NavigationArrow *third;
    struct NavigationArrow *fourth;
    register s32 invalid asm("r0");
    u8 *destination0;
    u8 *destination1, *destination2, *destination3;
    struct NavigationPage *page;
    struct NavigationPage **pages;
    u8 *selectedPage;
    first = &menu->arrows[0];
    first->flags |= 1;
    second = &menu->arrows[1];
    second->flags |= 1;
    third = &menu->arrows[2];
    third->flags |= 1;
    fourth = &menu->arrows[3];
    fourth->flags |= 1;
    destination0 = &menu->destinations[0];
    *destination0 = -1;
    destination1 = (u8 *)&menu->destinations[1];
    invalid = -1;
    *destination1 = invalid;
    destination2 = (u8 *)&menu->destinations[2];
    invalid = -1;
    *destination2 = invalid;
    destination3 = (u8 *)&menu->destinations[3];
    invalid = -1;
    *destination3 = invalid;
    pages = &menu->pages;
    selectedPage = &menu->page;
    page = &(*pages)[*selectedPage];
    if (menu->showAll) {

        if (page->neighbors[0] != -1) {
            if (GameState_GetMapRevealFlag((*pages)[page->neighbors[0]].map)) {
                *destination0 = page->neighbors[0];
                first->flags &= ~1;
                if ((1u & page->solidLinks) != 0)
                    first->flags &= ~0x200;
                else
                    first->flags |= 0x200;
            } else if (*selectedPage != menu->currentPage) {
                register u32 currentIndex asm("r6");
                if ((*pages)[page->neighbors[0]].map ==
                    (*pages)[currentIndex = menu->currentPage].map) {
                    *destination0 = currentIndex;
                    first->flags &= ~1;
                    if (page->neighbors[0] == menu->currentPage)
                        first->flags &= ~0x200;
                    else
                        first->flags |= 0x200;
                }
            }
        }

        if (page->neighbors[1] != -1) {
            if (GameState_GetMapRevealFlag(menu->pages[page->neighbors[1]].map)) {
                struct NavigationArrow *arrow;
                menu->destinations[1] = page->neighbors[1];
                arrow = &menu->arrows[1];
                arrow->flags &= ~1;
                if ((2u & page->solidLinks) != 0)
                    arrow->flags &= ~0x200;
                else
                    arrow->flags |= 0x200;
            } else {
                u8 *selected = &menu->page;
                u16 *current = &menu->currentPage;
                if (*selected != *current) {
                    register u32 currentIndex asm("r3");
                    if (menu->pages[page->neighbors[1]].map ==
                        menu->pages[currentIndex = *current].map) {
                        s8 *destination = &menu->destinations[1];
                        struct NavigationArrow *arrow;
                        *destination = currentIndex;
                        arrow = &menu->arrows[1];
                        arrow->flags &= ~1;
                        if (page->neighbors[1] == *current)
                            arrow->flags &= ~0x200;
                        else
                            arrow->flags |= 0x200;
                    }
                }
            }
        }

        if (page->neighbors[2] != -1) {
            if (GameState_GetMapRevealFlag(menu->pages[page->neighbors[2]].map)) {
                struct NavigationArrow *arrow;
                menu->destinations[2] = page->neighbors[2];
                arrow = &menu->arrows[2];
                arrow->flags &= ~1;
                if ((4u & page->solidLinks) != 0)
                    arrow->flags &= ~0x200;
                else
                    arrow->flags |= 0x200;
            } else {
                u8 *selected = &menu->page;
                u16 *current = &menu->currentPage;
                if (*selected != *current) {
                    register u32 currentIndex asm("r3");
                    if (menu->pages[page->neighbors[2]].map ==
                        menu->pages[currentIndex = *current].map) {
                        s8 *destination = &menu->destinations[2];
                        struct NavigationArrow *arrow;
                        *destination = currentIndex;
                        arrow = &menu->arrows[2];
                        arrow->flags &= ~1;
                        if (page->neighbors[2] == *current)
                            arrow->flags &= ~0x200;
                        else
                            arrow->flags |= 0x200;
                    }
                }
            }
        }

        if (page->neighbors[3] != -1) {
            if (GameState_GetMapRevealFlag(menu->pages[page->neighbors[3]].map)) {
                struct NavigationArrow *arrow;
                menu->destinations[3] = page->neighbors[3];
                arrow = &menu->arrows[3];
                arrow->flags &= ~1;
                if ((8u & page->solidLinks) != 0)
                    arrow->flags &= ~0x200;
                else
                    arrow->flags |= 0x200;
            } else {
                u8 *selected = &menu->page;
                u16 *current = &menu->currentPage;
                if (*selected != *current) {
                    register u32 currentIndex asm("r3");
                    if (menu->pages[page->neighbors[3]].map ==
                        menu->pages[currentIndex = *current].map) {
                        s8 *destination = &menu->destinations[3];
                        struct NavigationArrow *arrow;
                        *destination = currentIndex;
                        arrow = &menu->arrows[3];
                        arrow->flags &= ~1;
                        if (page->neighbors[3] == *current)
                            arrow->flags &= ~0x200;
                        else
                            arrow->flags |= 0x200;
                    }
                }
            }
        }

    } else {
        if (page->alternate[0] != -1) {
            if (GameState_GetMapRevealFlag((*pages)[page->alternate[0]].map)) {
                *destination0 = page->alternate[0];
                first->flags &= ~1;
                if ((page->solidLinks & 1u) && page->neighbors[0] == page->alternate[0])
                    first->flags &= ~0x200;
                else
                    menu->arrows[0].flags |= 0x200;
            }
        } else if (page->alternate[1] != -1) {
            if (GameState_GetMapRevealFlag((*pages)[page->alternate[1]].map)) {
                *destination1 = page->alternate[1];
                second->flags &= ~1;
                if ((page->solidLinks & 2u) && page->neighbors[1] == page->alternate[1])
                    second->flags &= ~0x200;
                else
                    menu->arrows[1].flags |= 0x200;
            }
        } else if (page->alternate[2] != -1) {
            if (GameState_GetMapRevealFlag((*pages)[page->alternate[2]].map)) {
                *destination2 = page->alternate[2];
                third->flags &= ~1;
                if ((page->solidLinks & 4u) && page->neighbors[2] == page->alternate[2])
                    third->flags &= ~0x200;
                else
                    menu->arrows[2].flags |= 0x200;
            }
        } else if (page->alternate[3] != -1) {
            if (GameState_GetMapRevealFlag((*pages)[page->alternate[3]].map)) {
                *destination3 = page->alternate[3];
                fourth->flags &= ~1;
                if ((page->solidLinks & 8u) && page->neighbors[3] == page->alternate[3])
                    fourth->flags &= ~0x200;
                else
                    menu->arrows[3].flags |= 0x200;
            }
        }
    }
}
