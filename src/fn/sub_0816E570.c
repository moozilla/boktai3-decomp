#include "global.h"
#include "scene_hud_display.h"
#include "ram.h"
#include "monster_particle_random.h"
s32 Mod(s32, s32);
void MessageWindow_Clear(void);
void ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_RequestHide(void);
void Sound_PlaySong(u32);
void SceneHud_GetActionIconPosition(struct SceneVec *, s32);
s32 Inventory_GetQuantityByStorage(u32, u32);
s32 Selection_GetDisplayFrame(s32);
void PauseMenu_BeginSpriteMotion(struct SceneHudState *, u32, u32, u32, struct SceneVec *,
                                 struct SceneVec *, s32, u32);
void Hud_SetInteractionCallback(u8 *, void (*)(void), u32);
void PauseMenu_OnCharacterSelectionUpdate(void);

/* The sample table as a halfword address: bit 0 of a u16 pointer is always clear. Built from
   its fields, the pointer has no REG_EQUAL note, so it keeps a register of its own
   (scope README facts 20 and 48). */
union RandomTableAddress {
    const u16 *table;
    struct {
        u32 odd : 1;
        u32 halfword : 31;
    } bits;
};
static inline const u16 *RandomTableAddress_Get(const union RandomTableAddress *address)
{
    return address->table;
}

void sub_0816E570(void *owner)
{
    struct SceneVec pos;
    struct SceneVec velocity;
    struct SceneHudState *ui = owner;
    u8 *kind;
    u16 speed;
    const u16 *table;

    kind = &ui->actionSelection;
    ui->categoryWidgets[*kind + 10].flags |= 1;
    SceneHud_GetActionIconPosition(&pos, *kind);
    pos.x += 8;
    pos.y += 8;
    {
        union RandomTableAddress address;
        address.bits.odd = 0;
        address.bits.halfword = RANDOM_TABLE_ADDR >> 1;
        table = RandomTableAddress_Get(&address);
    }
    velocity.x = -6 - Mod(ParticleRandom_ReadDirectSample(table), 3);
    velocity.y = -5 - Mod(ParticleRandom_ReadDirectSample(table), 5);
    speed = Mod(ParticleRandom_ReadDirectSample(table), 10) + 5;
    if (ParticleRandom_ReadDirectSample(table) & 1)
        speed = -speed;
    PauseMenu_BeginSpriteMotion(
        owner, 0,
        Selection_GetDisplayFrame(Inventory_GetQuantityByStorage(ui->actionCategory, *kind)),
        ui->categoryWidgets[*kind + 10].paletteBank, &pos, &velocity, 0, 0);
    {
        s16 *spin = (s16 *)&ui->widgetMotions[0].timer;
        *spin = speed;
    }
    MessageWindow_Clear();
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestHide();
    Sound_PlaySong(0x27d);
    Hud_SetInteractionCallback(owner, PauseMenu_OnCharacterSelectionUpdate, 1);
}
