#define SelectionPanel_ResetSecondarySelection sub_08169838

#include "global.h"
struct SelectionPanel {
    u8 pad[0xF4A];
    u8 selection;
};
void sub_08168BEC(struct SelectionPanel *);
void sub_081687D8(struct SelectionPanel *);
void sub_081685B4(struct SelectionPanel *);
void PauseMenu_ApplySelectedVariant(struct SelectionPanel *);
void sub_08169838(struct SelectionPanel *p)
{
    p->selection = 0;
    sub_08168BEC(p);
    sub_081687D8(p);
    sub_081685B4(p);
    PauseMenu_ApplySelectedVariant(p);
}
