#include "menus/scene_hud/scene_hud_display.h"

// Refresh the four map navigation arrows and record their destination pages.
#define MapMenu_RefreshNavigationArrows sub_0816BC54
u32 GameState_GetMapRevealFlag(u32);
void MapMenu_RefreshNavigationArrows(struct SceneHudState *menu)
{
    struct SceneHudNavigationPage *page;
    menu->navigationArrows[0].flags |= 1;
    menu->navigationArrows[1].flags |= 1;
    menu->navigationArrows[2].flags |= 1;
    menu->navigationArrows[3].flags |= 1;
    menu->navigationDestinations[0] = -1;
    menu->navigationDestinations[1] = -1;
    menu->navigationDestinations[2] = -1;
    menu->navigationDestinations[3] = -1;
    page = &menu->navigationPages[menu->navigationPage];
    if (menu->showAllMapLinks) {
        if (page->neighbors[0] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->neighbors[0]].map)) {
                menu->navigationDestinations[0] = page->neighbors[0];
                menu->navigationArrows[0].flags &= ~1;
                if (page->solidLinks & 1)
                    menu->navigationArrows[0].flags &= ~0x200;
                else
                    menu->navigationArrows[0].flags |= 0x200;
            } else if (menu->navigationPage != menu->currentMapPage) {
                if (menu->navigationPages[page->neighbors[0]].map ==
                    menu->navigationPages[menu->currentMapPage].map) {
                    menu->navigationDestinations[0] = menu->currentMapPage;
                    menu->navigationArrows[0].flags &= ~1;
                    if (page->neighbors[0] == menu->currentMapPage)
                        menu->navigationArrows[0].flags &= ~0x200;
                    else
                        menu->navigationArrows[0].flags |= 0x200;
                }
            }
        }
        if (page->neighbors[1] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->neighbors[1]].map)) {
                menu->navigationDestinations[1] = page->neighbors[1];
                menu->navigationArrows[1].flags &= ~1;
                if (page->solidLinks & 2)
                    menu->navigationArrows[1].flags &= ~0x200;
                else
                    menu->navigationArrows[1].flags |= 0x200;
            } else if (menu->navigationPage != menu->currentMapPage) {
                if (menu->navigationPages[page->neighbors[1]].map ==
                    menu->navigationPages[menu->currentMapPage].map) {
                    menu->navigationDestinations[1] = menu->currentMapPage;
                    menu->navigationArrows[1].flags &= ~1;
                    if (page->neighbors[1] == menu->currentMapPage)
                        menu->navigationArrows[1].flags &= ~0x200;
                    else
                        menu->navigationArrows[1].flags |= 0x200;
                }
            }
        }
        if (page->neighbors[2] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->neighbors[2]].map)) {
                menu->navigationDestinations[2] = page->neighbors[2];
                menu->navigationArrows[2].flags &= ~1;
                if (page->solidLinks & 4)
                    menu->navigationArrows[2].flags &= ~0x200;
                else
                    menu->navigationArrows[2].flags |= 0x200;
            } else if (menu->navigationPage != menu->currentMapPage) {
                if (menu->navigationPages[page->neighbors[2]].map ==
                    menu->navigationPages[menu->currentMapPage].map) {
                    menu->navigationDestinations[2] = menu->currentMapPage;
                    menu->navigationArrows[2].flags &= ~1;
                    if (page->neighbors[2] == menu->currentMapPage)
                        menu->navigationArrows[2].flags &= ~0x200;
                    else
                        menu->navigationArrows[2].flags |= 0x200;
                }
            }
        }
        if (page->neighbors[3] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->neighbors[3]].map)) {
                menu->navigationDestinations[3] = page->neighbors[3];
                menu->navigationArrows[3].flags &= ~1;
                if (page->solidLinks & 8)
                    menu->navigationArrows[3].flags &= ~0x200;
                else
                    menu->navigationArrows[3].flags |= 0x200;
            } else if (menu->navigationPage != menu->currentMapPage) {
                if (menu->navigationPages[page->neighbors[3]].map ==
                    menu->navigationPages[menu->currentMapPage].map) {
                    menu->navigationDestinations[3] = menu->currentMapPage;
                    menu->navigationArrows[3].flags &= ~1;
                    if (page->neighbors[3] == menu->currentMapPage)
                        menu->navigationArrows[3].flags &= ~0x200;
                    else
                        menu->navigationArrows[3].flags |= 0x200;
                }
            }
        }
    } else {
        if (page->alternate[0] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->alternate[0]].map)) {
                menu->navigationDestinations[0] = page->alternate[0];
                menu->navigationArrows[0].flags &= ~1;
                if ((page->solidLinks & 1) && page->neighbors[0] == page->alternate[0])
                    menu->navigationArrows[0].flags &= ~0x200;
                else
                    menu->navigationArrows[0].flags |= 0x200;
            }
        } else if (page->alternate[1] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->alternate[1]].map)) {
                menu->navigationDestinations[1] = page->alternate[1];
                menu->navigationArrows[1].flags &= ~1;
                if ((page->solidLinks & 2) && page->neighbors[1] == page->alternate[1])
                    menu->navigationArrows[1].flags &= ~0x200;
                else
                    menu->navigationArrows[1].flags |= 0x200;
            }
        } else if (page->alternate[2] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->alternate[2]].map)) {
                menu->navigationDestinations[2] = page->alternate[2];
                menu->navigationArrows[2].flags &= ~1;
                if ((page->solidLinks & 4) && page->neighbors[2] == page->alternate[2])
                    menu->navigationArrows[2].flags &= ~0x200;
                else
                    menu->navigationArrows[2].flags |= 0x200;
            }
        } else if (page->alternate[3] != -1) {
            if (GameState_GetMapRevealFlag(menu->navigationPages[page->alternate[3]].map)) {
                menu->navigationDestinations[3] = page->alternate[3];
                menu->navigationArrows[3].flags &= ~1;
                if ((page->solidLinks & 8) && page->neighbors[3] == page->alternate[3])
                    menu->navigationArrows[3].flags &= ~0x200;
                else
                    menu->navigationArrows[3].flags |= 0x200;
            }
        }
    }
}
