#include "scene_hud_display.h"

// MATCHING_CONCESSION: register-pinning

// Refresh the four map navigation arrows and record their destination pages.
#define MapMenu_RefreshNavigationArrows sub_0816BC54
u32 GameState_GetMapRevealFlag(u32);
void MapMenu_RefreshNavigationArrows(struct SceneHudState *menu)
{
    u32 *first, *second, *third, *fourth;
    register s32 invalid asm("r0");
    u8 *destination0;
    u8 *destination1, *destination2, *destination3;
    struct SceneHudNavigationPage *page;
    struct SceneHudNavigationPage **pages;
    u8 *selectedPage;
    first = &menu->navigationArrows[0].flags;
    *first |= 1;
    second = &menu->navigationArrows[1].flags;
    *second |= 1;
    third = &menu->navigationArrows[2].flags;
    *third |= 1;
    fourth = &menu->navigationArrows[3].flags;
    *fourth |= 1;
    destination0 = &menu->navigationDestinations[0];
    *destination0 = -1;
    destination1 = (u8 *)&menu->navigationDestinations[1];
    invalid = -1;
    *destination1 = invalid;
    destination2 = (u8 *)&menu->navigationDestinations[2];
    invalid = -1;
    *destination2 = invalid;
    destination3 = (u8 *)&menu->navigationDestinations[3];
    invalid = -1;
    *destination3 = invalid;
    pages = &menu->navigationPages;
    selectedPage = &menu->navigationPage;
    page = &(*pages)[*selectedPage];
    if (menu->showAllMapLinks) {

        if (page->neighbors[0] != -1) {
            if (GameState_GetMapRevealFlag((*pages)[page->neighbors[0]].map)) {
                *destination0 = page->neighbors[0];
                *first &= ~1;
                if ((1u & page->solidLinks) != 0)
                    *first &= ~0x200;
                else
                    *first |= 0x200;
            } else if (*selectedPage != menu->currentMapPage) {
                register u32 currentIndex asm("r6");
                if ((*pages)[page->neighbors[0]].map ==
                    (*pages)[currentIndex = menu->currentMapPage].map) {
                    *destination0 = currentIndex;
                    *first &= ~1;
                    if (page->neighbors[0] == menu->currentMapPage)
                        *first &= ~0x200;
                    else
                        *first |= 0x200;
                }
            }
        }

        if (page->neighbors[1] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->neighbors[1]].map)) {
                u32 *arrow;
                menu->navigationDestinations[1] = page->neighbors[1];
                arrow = &menu->navigationArrows[1].flags;
                *arrow &= ~1;
                if ((2u & page->solidLinks) != 0)
                    *arrow &= ~0x200;
                else
                    *arrow |= 0x200;
            } else {
                u8 *selected = &menu->navigationPage;
                u16 *current = &menu->currentMapPage;
                if (*selected != *current) {
                    register u32 currentIndex asm("r3");
                    if (menu->navigationPages[page->neighbors[1]].map ==
                        menu->navigationPages[currentIndex = *current].map) {
                        s8 *destination = &menu->navigationDestinations[1];
                        u32 *arrow;
                        *destination = currentIndex;
                        arrow = &menu->navigationArrows[1].flags;
                        *arrow &= ~1;
                        if (page->neighbors[1] == *current)
                            *arrow &= ~0x200;
                        else
                            *arrow |= 0x200;
                    }
                }
            }
        }

        if (page->neighbors[2] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->neighbors[2]].map)) {
                u32 *arrow;
                menu->navigationDestinations[2] = page->neighbors[2];
                arrow = &menu->navigationArrows[2].flags;
                *arrow &= ~1;
                if ((4u & page->solidLinks) != 0)
                    *arrow &= ~0x200;
                else
                    *arrow |= 0x200;
            } else {
                u8 *selected = &menu->navigationPage;
                u16 *current = &menu->currentMapPage;
                if (*selected != *current) {
                    register u32 currentIndex asm("r3");
                    if (menu->navigationPages[page->neighbors[2]].map ==
                        menu->navigationPages[currentIndex = *current].map) {
                        s8 *destination = &menu->navigationDestinations[2];
                        u32 *arrow;
                        *destination = currentIndex;
                        arrow = &menu->navigationArrows[2].flags;
                        *arrow &= ~1;
                        if (page->neighbors[2] == *current)
                            *arrow &= ~0x200;
                        else
                            *arrow |= 0x200;
                    }
                }
            }
        }

        if (page->neighbors[3] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->neighbors[3]].map)) {
                u32 *arrow;
                menu->navigationDestinations[3] = page->neighbors[3];
                arrow = &menu->navigationArrows[3].flags;
                *arrow &= ~1;
                if ((8u & page->solidLinks) != 0)
                    *arrow &= ~0x200;
                else
                    *arrow |= 0x200;
            } else {
                u8 *selected = &menu->navigationPage;
                u16 *current = &menu->currentMapPage;
                if (*selected != *current) {
                    register u32 currentIndex asm("r3");
                    if (menu->navigationPages[page->neighbors[3]].map ==
                        menu->navigationPages[currentIndex = *current].map) {
                        s8 *destination = &menu->navigationDestinations[3];
                        u32 *arrow;
                        *destination = currentIndex;
                        arrow = &menu->navigationArrows[3].flags;
                        *arrow &= ~1;
                        if (page->neighbors[3] == *current)
                            *arrow &= ~0x200;
                        else
                            *arrow |= 0x200;
                    }
                }
            }
        }

    } else {
        if (page->alternate[0] != -1) {
            if (GameState_GetMapRevealFlag((*pages)[page->alternate[0]].map)) {
                *destination0 = page->alternate[0];
                *first &= ~1;
                if ((page->solidLinks & 1u) && page->neighbors[0] == page->alternate[0])
                    *first &= ~0x200;
                else
                    menu->navigationArrows[0].flags |= 0x200;
            }
        } else if (page->alternate[1] != -1) {
            if (GameState_GetMapRevealFlag((*pages)[page->alternate[1]].map)) {
                *destination1 = page->alternate[1];
                *second &= ~1;
                if ((page->solidLinks & 2u) && page->neighbors[1] == page->alternate[1])
                    *second &= ~0x200;
                else
                    menu->navigationArrows[1].flags |= 0x200;
            }
        } else if (page->alternate[2] != -1) {
            if (GameState_GetMapRevealFlag((*pages)[page->alternate[2]].map)) {
                *destination2 = page->alternate[2];
                *third &= ~1;
                if ((page->solidLinks & 4u) && page->neighbors[2] == page->alternate[2])
                    *third &= ~0x200;
                else
                    menu->navigationArrows[2].flags |= 0x200;
            }
        } else if (page->alternate[3] != -1) {
            if (GameState_GetMapRevealFlag((*pages)[page->alternate[3]].map)) {
                *destination3 = page->alternate[3];
                *fourth &= ~1;
                if ((page->solidLinks & 8u) && page->neighbors[3] == page->alternate[3])
                    *fourth &= ~0x200;
                else
                    menu->navigationArrows[3].flags |= 0x200;
            }
        }
    }
}
