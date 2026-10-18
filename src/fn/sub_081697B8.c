#define SelectionPanel_RefreshAttachedPanel  sub_081697B8
#define SelectionPanel_RefreshPrimaryEntries PauseMenu_RefreshEquipmentSlotIcons
#include "global.h"
struct SelectionPanel {
    u8 pad[0xA40];
    void *owner;
};
extern u32 gUnk_030053F4;
void sub_08169504(struct SelectionPanel *);
void sub_08168850(struct SelectionPanel *);
void sub_08168A0C(struct SelectionPanel *);
void sub_08168BEC(struct SelectionPanel *);
void sub_081686FC(struct SelectionPanel *);
void PauseMenu_RefreshEquipmentSlotIcons(struct SelectionPanel *);
void sub_081687D8(struct SelectionPanel *);
void sub_08168534(struct SelectionPanel *);
void sub_081685B4(struct SelectionPanel *);
void sub_081697B8(struct SelectionPanel *p)
{
    sub_08169504(p);
    if (p->owner) {
        sub_08168850(p);
        sub_08168A0C(p);
        sub_08168BEC(p);
        sub_081686FC(p);
        PauseMenu_RefreshEquipmentSlotIcons(p);
        sub_081687D8(p);
        if (gUnk_030053F4 & 0x1000)
            sub_08168534(p);
        else
            sub_081685B4(p);
    }
}
