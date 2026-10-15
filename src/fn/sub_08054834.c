// MATCHING_CONCESSION: register-pinning
#include "hit_switch.h"
void sub_08225AB4(struct SceneBody *, u32, struct SceneVec *, u32, u32, struct HitSwitchEntry *);
void sub_08003C00(struct SceneBody *);
void HitSwitch_InitEntryFromScript(struct HitSwitchManager *input)
{
    struct HitSwitchManager *manager = input;
    struct HitSwitchEntry *entry;
    struct SceneVec *world;
    struct SceneAnimation *animation;
    struct HitSwitchSprite *sprite;
    u32 present;
    {
        u32 count = manager->count;
        register u32 offset asm("r0") = count * 0x118 + 0x3C;
        entry = (struct HitSwitchEntry *)((u8 *)manager + offset);
    }
    {
        u32 found = Script_SeekToKeyword('n');
        if (found)
            entry->collisionId = Script_GetValue();
        else
            entry->collisionId = found;
    }
    present = Script_SeekToKeyword('p');
    if (present) {
        entry->world.x = Script_GetValue();
        entry->world.y = Script_GetValue();
        entry->world.z = Script_GetValue();
        world = &entry->world;
    } else {
        entry->world.x = present;
        entry->world.y = present;
        entry->world.z = present;
        world = &entry->world;
    }
    {
        s32 x = (s16)(u16)world->x >> 8;
        s32 z = (s16)(u16)world->z >> 8;
        u32 index;
        if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
            index = 0;
        else
            index = gUnk_030052F4->rows[z] + x;
        entry->cellIndex = index;
    }
    if (Script_SeekToKeyword('t'))
        entry->kind = Script_GetValue();
    else
        entry->kind = 0;
    {
        u32 kind = entry->kind;
        u32 resource;
        resource = 0xD166;
        if (kind == 0)
            resource = 0xC3C3;
        sprite = &entry->sprite;
        animation = &entry->animation;
        AnimData_Load(animation, resource);
        Sprite_Init(sprite, animation, 0);
        sprite->renderMode = 3;
    }
    if (Script_SeekToKeyword('r') && Script_GetValue())
        sprite->flags |= 4;
    sprite->position = *world;
    if (Script_SeekToKeyword('h')) {
        entry->bodyEnabled = Script_GetValue();
        if (entry->bodyEnabled) {
            struct SceneBody *body = &entry->body;
            sub_08225AB4(body, entry->collisionId, &sprite->position, 0, 7, entry);
            sub_08003C00(body);
        }
    } else
        entry->bodyEnabled = 0;
    entry->hitCount = 0;
    if (Script_SeekToKeyword('A'))
        entry->requiredHits = Script_GetValue();
    else
        entry->requiredHits = 10;
    {
        u32 period = Script_SeekToKeyword('W');
        if (period)
            period = Script_GetValue();
        entry->bobPeriod = period;
    }
    HitSwitch_InitCollider(entry);
    {
        u8 *cooldown = &entry->hitCooldown;
        register u32 zero asm("r0") = 0;
        *cooldown = zero;
    }
    {
        register u32 found asm("r1") = Script_SeekToKeyword('s');
        if (found)
            entry->completed = Script_GetValue();
        else
            entry->completed = found;
    }
    if (entry->completed) {
        u16 *state;
        u32 flags, mask;
        HitSwitch_RegisterCompletedCell(entry);
        state = &entry->collider.state;
        flags = *state;
        mask = 4;
        mask |= flags;
        *state = mask;
        sprite->frame = 2;
        if (entry->kind == 0) {
            sprite->renderMode = 2;
            HitSwitch_SetUpdate(entry, HitSwitch_TickCompletedStatic);
        } else
            HitSwitch_SetUpdate(entry, HitSwitch_UpdateCompletedBobbing);
    } else {
        entry->cellRegistered = 0;
        sprite->frame = 0;
        sprite->renderMode = 3;
        if (entry->kind == 0)
            HitSwitch_SetUpdate(entry, HitSwitch_UpdateStatic);
        else
            HitSwitch_SetUpdate(entry, HitSwitch_UpdateBobbing);
    }
    if (Script_SeekToKeyword('g')) {
        entry->tag = Script_GetValue();
        entry->completedScriptId = Script_GetValue();
    } else {
        entry->tag = -1;
        entry->completedScriptId = 0;
    }
    {
        u32 count = manager->count + 1;
        manager->count = count;
    }
}
