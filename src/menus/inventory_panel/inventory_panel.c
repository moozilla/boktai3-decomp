#include "engine/shared/global.h"
#include "engine/shared/scene_item_values.h"
#include "system/game_state/game_state.h"
#include "player/player.h"
#include "menus/inventory_panel/inventory_panel.h"
#include "menus/scene_hud/scene_hud_display.h"
#include "engine/shared/monster_particle_random.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "bosses/vanargand_projectile/vanargand_projectiles.h"

/* Tentative inventory display family, inferred from slot and item consumers. */

s32 GameState_GetUnlockEntry(s32);

void ActionRecord_CopyFromEntry(struct SceneValueEquipment *, void *);

void *Player_CalculatePreviewActionStat(void *, s32, void *, struct SceneValueEquipment *, s32);

s32 InventoryPanel_CreateEntry(void **out, s32 position, s32 index, s32 slotMode, s32 kind)
{
    s32 result;
    u8 data[0x20];
    struct SceneValueEquipment *equipment = 0;
    if (slotMode) {
        if (index > 7) {
            kind = 4;
            data[0] = GameState_GetUnlockEntry(
                          ((struct GameState *)((u8 *)gGameState + index * 2))->inventory[0]) -
                      1;
        } else {
            kind = 0;
            equipment = Inventory_GetEquipmentEntry(
                ((struct GameState *)((u8 *)gGameState + index * 2))->inventory[0]);
            ActionRecord_CopyFromEntry(equipment, data);
        }
    } else if (kind == 4) {
        data[0] = GameState_GetUnlockEntry(index) - 1;
    } else {
        kind = 0;
        equipment = Inventory_GetEquipmentEntry(index);
        ActionRecord_CopyFromEntry(equipment, data);
    }
    if (data[0] != 255) {
        *out = Player_CalculatePreviewActionStat((*(void **)gPlayers), position, data, equipment,
                                                 kind);
        result = kind;
    } else {
        result = -1;
    }
    return result;
}

struct MenuDigit {
    u8 unknown0[8];
    u32 flags;
    u8 unknownC[0x14];
    s16 x, y;
    u8 unknown24[0x3C];
};

struct MenuDisplay {
    u8 unknown0[0x1420];
    struct MenuDigit digits[1];
};

void Hud_FillDigitCells(s32, s32, s32, s32);

void MenuDisplay_LoadDecimalDigits(struct MenuDisplay *, s32, s32, s32, s32);

void InventoryPanel_SetEntryRow(struct MenuDisplay *p, s32 x, s32 y, s32 value, s32 frameBase,
                                s32 first)
{
    s32 i;
    if (value < 0) {
        for (i = 0; i < 3; i++)
            p->digits[first + i].flags |= 1;
        Hud_FillDigitCells(3, x, y, 0);
    } else {
        Hud_FillDigitCells(3, x, y, -1);
        for (i = 0; i < 3; i++) {
            p->digits[first + i].x = x * 8 + i * 8;
            p->digits[first + i].y = y * 8;
            p->digits[first + i].flags &= ~1;
        }
        MenuDisplay_LoadDecimalDigits(p, value, 100, frameBase, first);
    }
}

/* Tentative inventory display family, inferred from slot and item consumers. */

s32 InventoryPanel_CreateEntry(void **, s32, s32, s32, s32);

void InventoryPanel_SetEntryRow(struct MenuDisplay *, s32, s32, s32, s32, s32);

static inline void HideSelected(struct InventoryPanel *p, s32 x, s32 y)
{
    s32 i;
    for (i = 0; i < 3; i++)
        p->selected[i].flags |= 1;
    Hud_FillDigitCells(3, x, y, 0);
}

s32 InventoryPanel_UpdateSelectedUnlock(struct InventoryPanel *p, s32 x, s32 y, s32 query,
                                        s32 position)
{
    void *value;
    s32 kind = InventoryPanel_CreateEntry(&value, position, (s16)gGameState->unk50, 1, 0);
    if (query) {
        if (kind != 4) {
            kind = -1;
            goto done;
        }
    } else {
        if (kind != 4) {
            HideSelected(p, x, y);
            return -1;
        }
        if ((s32)value > 0)
            InventoryPanel_SetEntryRow((struct MenuDisplay *)p, x, y, (s32)value, query, 0x47);
        else
            HideSelected(p, x, y);
    }
    kind = (s32)value;
done:
    return kind;
}

/* Tentative inventory display family, inferred from slot and item consumers. */

s32 InventoryPanel_UpdateSelectedEquipment(struct InventoryPanel *p, s32 x, s32 y, s32 query,
                                           s32 position)
{
    void *value;
    s32 kind = InventoryPanel_CreateEntry(&value, position, (s16)gGameState->unk50, 1, 0);
    if (query) {
        if (kind != 0) {
            kind = -1;
            goto done;
        }
    } else {
        if (kind != 0) {
            Hud_FillDigitCells(3, x, y, 0);
            return -1;
        }
        if ((s32)value > 0)
            InventoryPanel_SetEntryRow((struct MenuDisplay *)p, x, y, (s32)value, kind, 0x47);
        else
            HideSelected(p, x, y);
    }
    kind = (s32)value;
done:
    return kind;
}

s32 InventoryPanel_UpdateSelectedUnlock(struct InventoryPanel *, s32, s32, s32, s32);

s32 InventoryPanel_UpdateSelectedEquipment(struct InventoryPanel *, s32, s32, s32, s32);

static inline void DrawComparison(void *p, s32 x, s32 y, s32 value, s32 delta, s32 old)
{
    if (delta > 0)
        InventoryPanel_SetEntryRow((struct MenuDisplay *)p, x, y, value, 10, 0x47);
    else if (delta < 0)
        InventoryPanel_SetEntryRow((struct MenuDisplay *)p, x, y, value, 20, 0x47);
    else
        InventoryPanel_SetEntryRow((struct MenuDisplay *)p, x, y, old, 0, 0x47);
}

void InventoryPanel_DrawStatComparison(u8 *p, s32 unlock, s32 equipment, s32 selected, s32 active,
                                       s32 position)
{
    s32 value, old, delta, item, kind;
    if (active) {
        if ((s16)gGameState->unk50 <= 7) {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 0;
        } else {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 4;
        }
        if (InventoryPanel_CreateEntry(&value, position, item, 0, kind) < 0) {
            InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, 0);
            InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, 0);
        } else if (kind == 0) {
            old = InventoryPanel_UpdateSelectedEquipment(p, 0, 0, 1, 0);
            delta = value - old;
            Hud_FillDigitCells(3, 8, 3, 0);
            DrawComparison(p, 8, 4, value, delta, old);
        } else {
            old = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, 0);
            delta = value - old;
            Hud_FillDigitCells(3, 8, 4, 0);
            DrawComparison(p, 8, 3, value, delta, old);
        }
    } else if (selected) {
        if ((s16)gGameState->unk50 <= 7) {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 0;
        } else {
            item =
                ((struct GameState *)((s16)gGameState->unk50 * 2 + (u32)gGameState))->inventory[0];
            kind = 4;
        }
        if (InventoryPanel_CreateEntry(&value, position, item, 0, kind) < 0) {
            InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, 0);
            InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, 0);
        } else if (kind == 0) {
            old = InventoryPanel_UpdateSelectedEquipment(p, 0, 0, 1, 0);
            delta = value - old;
            Hud_FillDigitCells(3, 8, 3, 0);
            DrawComparison(p, 8, 4, value, delta, old);
        } else {
            old = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, 0);
            delta = value - old;
            Hud_FillDigitCells(3, 8, 4, 0);
            DrawComparison(p, 8, 3, value, delta, old);
        }
    } else if (equipment) {
        if (InventoryPanel_CreateEntry(&value, position, p[0x4387], 0, 0) < 0) {
            InventoryPanel_UpdateSelectedUnlock(p, 8, 3, 0, 0);
            InventoryPanel_UpdateSelectedEquipment(p, 8, 4, 0, 0);
        } else {
            old = InventoryPanel_UpdateSelectedEquipment(p, 0, 0, 1, 0);
            delta = value - old;
            Hud_FillDigitCells(3, 8, 3, 0);
            DrawComparison(p, 8, 4, value, delta, old);
        }
    } else if (unlock) {
        if (InventoryPanel_CreateEntry(&value, 0, p[0x4387] - 8, 0, 4) < 0)
            InventoryPanel_UpdateSelectedUnlock(p, 3, 14, 0, 0);
        else {
            old = InventoryPanel_UpdateSelectedUnlock(p, 0, 0, 1, 0);
            delta = value - old;
            DrawComparison(p, 3, 14, value, delta, old);
        }
    }
}

#define STATUS_WIDGET 79

static inline u32 Offset(u32 base, u32 off)
{
    u32 sum = base + off;
    return sum;
}

static inline void PlaceStatusIcon(struct SceneHudState *p, s32 n,
                                   struct SceneInputPackedPosition pos)
{
    u32 off = (n + STATUS_WIDGET) * sizeof(struct SceneInputWidget);
    *(u32 *)Offset((u32)&p->categoryWidgets[0].flags, off) &= ~1;
    *(struct SceneInputPackedPosition *)Offset((u32)&p->categoryWidgets[0].position, off) = pos;
}

void InventoryPanel_ShowStatusIcons(struct SceneHudState *p)
{
    struct SceneInputPackedPosition pos;
    s32 n = 0;
    pos.x = 0;
    pos.y = 0;
    pos.z = 0;
    if (((struct GameState *)(p->actionOwner->timedEventSlot * 4 + (u32)gGameState))
            ->counters628[0] > 0) {
        Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5e, 1);
        p->statusIcons[n].flags &= ~1;
        *(struct SceneInputPackedPosition *)&p->statusIcons[n].position = pos;
        pos.x = 0xd;
        n = 1;
    }
    if (p->actionOwner->movementModifierActive) {
        if (p->actionOwner->movementModifierKind == 1)
            Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5c, 1);
        else if (p->actionOwner->movementModifierKind == 2)
            Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5b, 1);
        else
            Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5d, 1);
        PlaceStatusIcon(p, n, pos);
        pos.x += 0xd;
        n++;
    } else if (p->actionOwner->scriptStatus[0]) {
        Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x5f, 1);
        PlaceStatusIcon(p, n, pos);
        pos.x += 0xd;
        n++;
    }
    if (p->actionOwner->scriptStatus[1]) {
        Anim_LoadKeyframe(&p->statusIcons[n], p->widgetResource, 0x60, 1);
        PlaceStatusIcon(p, n, pos);
        n++;
    }
    for (; n <= 3; n++)
        p->statusIcons[n].flags |= 1;
}

u32 MenuParticle_UpdateMotion(struct InventoryPanelCallback *particle, void *context)
{
    if (++particle->state.age > 19) {
        particle->sprite.flags |= 1;
        particle->state.active = 0;
    } else {
        particle->sprite.position.x += particle->velocityX;
        particle->sprite.position.y += particle->velocityY;
        if (!(particle->state.age & 3))
            particle->velocityY++;
    }
    return particle->state.active;
}

/* Tentative inventory display family, inferred from slot and item consumers. */

void InventoryPanel_UpdateCallbackGroups(struct InventoryPanel *panel)
{
    s32 i = 0;
    do {
        struct InventoryPanelCallbackGroup *group;
        s32 enabled;
        s32 next;
        group = &panel->groups[i];
        enabled = group->active;
        next = i + 1;
        if (enabled) {
            s32 active = 0;
            struct InventoryPanelCallbackState *state = &group->entries[0].state;
            struct InventoryPanelCallback *entry = group->entries;
            s32 j = 7;
            do {
                if (state->active &&
                    sub_08249240(entry, group->context, state->callback))
                    active = 1;
                state = (struct InventoryPanelCallbackState *)((u8 *)state + sizeof(*entry));
                entry++;
            } while (--j >= 0);
            group->active = active;
        }
        i = next;
    } while (i <= 1);
}

/* Tentative inventory display family, inferred from slot and item consumers. */

extern const s16 gUnk_086149C4[];

void *AnimData_FindEntry(s32);

static inline u32 Arg1(u32 k_, s32 angle)
{
    return angle + k_;
}

void InventoryPanel_StartCallbackBurst(struct InventoryPanel *panel, s32 index, s32 x, s32 y)
{
    struct InventoryPanelCallbackGroup *group = &panel->groups[index];
    struct InventoryPanelCallbackState *state;
    struct InventoryPanelCallback *entry;
    struct SpriteDrawable *sprite;
    s32 i;
    group->context = AnimData_FindEntry(0x1C1C);
    group->active = 1;
    i = 0;
    {
        const u16 *table = gRandomTable;
        state = &group->entries[0].state;
        entry = group->entries;
        sprite = &entry->sprite;
        for (; i < 8; i++) {
            s32 angle;
            s32 scale;
            u32 flags = entry->sprite.flags & (~1);
            entry->sprite.flags = flags;
            if ((u8)ParticleRandom_ReadSample(table) & 1)
                flags |= 4;
            else
                flags &= ~4;
            entry->sprite.flags = flags;
            SpriteDrawable_SelectTileFrame(sprite, group->context, 0x15);
            Effect_SetPalette(sprite, 0);
            angle = ((i * 32) + (((s32)ParticleRandom_ReadSample(table) >> 3) & 0xf)) - 8;
            scale = (((s32)ParticleRandom_ReadDirectSample(table) >> 3) & 1) + 3;
            entry->velocityX = VanargandProjectile_Scale(gUnk_086149C4[Arg1(0x40, angle) & 0xff] * scale);
            entry->velocityY = VanargandProjectile_Scale(gUnk_086149C4[angle & 0xff] * scale) - 2;
            entry->sprite.position.x = entry->velocityX + x;
            entry->sprite.position.y = entry->velocityY + y;
            state->active = 1;
            state->age = 0;
            entry->state.callback = MenuParticle_UpdateMotion;
            state = (struct InventoryPanelCallbackState *)((u8 *)state + sizeof(*entry));
            entry++;
            sprite = (struct SpriteDrawable *)((u8 *)sprite + sizeof(*entry));
        }
    }
}

void PlayerPanel_RemoveEffects(struct InventoryPanel *panel)
{
    s32 group = 0;
    u32 offset = 0;
    s32 i;
    do {
        struct InventoryPanelCallback *particle =
            (struct InventoryPanelCallback *)((u8 *)panel + (offset + 0x4458));
        for (i = 7; i >= 0; i--) {
            Effect_Remove(&particle->sprite);
            particle++;
        }
        offset += sizeof(struct InventoryPanelCallbackGroup);
        (*(&group))++;
    } while (group < 2);
    {
        struct SpriteDrawable *effect = panel->numberDigits;
        for (group = 3; group >= 0; group--) {
            Effect_Remove(effect);
            effect++;
        }
    }
}

/* Tentative inventory display family, inferred from slot and item consumers. */

void InventoryPanel_InitCallbackGroups(struct InventoryPanel *panel)
{
    s32 i, j;
    for (i = 0; i <= 1; i++) {
        struct InventoryPanelCallbackGroup *group = &panel->groups[i];
        s32 frame = 0x91;
        if (i == 0)
            frame = 0x11;
        group->context = AnimData_FindEntry(0x1C1E);
        group->active = 0;
        group->initialized = 0;
        for (j = 0; j < 8; j++) {
            ParticleSprite_Initialize(&group->entries[j].sprite, group->context, frame);
            Effect_SetOrigin(&group->entries[j].sprite, -4, -4);
            group->entries[j].sprite.priority = 0;
            group->entries[j].state.active = 0;
        }
    }
}
