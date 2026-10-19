#define SelectionPanel_RemoveEntryAnimations sub_08169738

#include "global.h"
#include "engine.h"
struct PanelAnimation {
    u8 bytes[0x60];
};
struct SelectionPanel {
    u8 pad[0xC88];
    u8 primary[3][0x60];
    u8 padDA8[0x80];
    u8 middle[3][0x60];
    u8 padF48[0x28];
    u8 secondary[3][0x60];
    u8 markers[3][0x60];
    u8 middleMarkers[3][0x60];
    u8 lastMarkers[3][0x60];
};
void sub_08169738(struct SelectionPanel *p)
{
    struct PanelAnimation *a, *b;
    s32 i;
    struct PanelAnimation *c;
    c = (struct PanelAnimation *)p->secondary[0];
    b = (struct PanelAnimation *)p->middle[0];
    a = (struct PanelAnimation *)p->primary[0];
    for (i = 2; i >= 0; i--) {
        Anim_Remove(a);
        Anim_Remove(b);
        Anim_Remove(c);
        c++;
        b++;
        a++;
    }
    c = (struct PanelAnimation *)p->lastMarkers[0];
    b = (struct PanelAnimation *)p->middleMarkers[0];
    a = (struct PanelAnimation *)p->markers[0];
    for (i = 2; i >= 0; i--) {
        Anim_Remove(a);
        Anim_Remove(b);
        Anim_Remove(c);
        c++;
        b++;
        a++;
    }
}
