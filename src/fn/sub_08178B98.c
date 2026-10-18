#define Hud_UpdateWithCallback sub_08178B98
#define gPlayerSlots gUnk_02000580
#include "global.h"
#include "player.h"
struct FieldHud { u8 unknown0[0xA40]; struct Player *player; u8 unknownA44[12]; void *updateCallback; };
s32 sub_0824923C(void *, void *);
void sub_0816755C(struct FieldHud *);
s32 sub_08178B98(struct FieldHud *hud)
{
    struct Player *player = gUnk_02000580[0];
    void *callback;
    hud->player = player;
    callback = hud->updateCallback;
    if (callback)
        sub_0824923C(hud, callback);
    sub_0816755C(hud);
    return 0;
}
