#include "directional_interaction.h"
// MATCHING_CONCESSION: register-pinning

/* Sets the controller's state and flags it as newly entered. */
u32 DirectionalController_SetState(struct DirectionalController *p, u32 state)
{
    p->state = state;
    p->entered = 1;
}

/* Returns 1 once after a state starts. */
u32 DirectionalController_ConsumeEntered(struct DirectionalController *p)
{
    u32 r;
    if (p->entered == 0) {
        r = 0;
    } else {
        p->entered = 0;
        r = 1;
    }
    return r;
}

s32 DirectionalController_InitSlot(struct DirectionalManager *manager,
                                   struct DirectionalController *p, s32 index)
{
    void *animation = p->animation;
    s32 r;
    if (AnimData_Load(animation, 0xA850) != 0) {
        struct DirectionalDrawable *q = &p->drawable;
        Sprite_Init(q, animation, 1);
        q->scale = 1;
        p->drawable.mode = 2;
        r = 0;
    } else
        r = -1;
    return r;
}

/* Finds the enabled controller with the given script id; optionally returns its slot index. */
struct DirectionalController *
DirectionalController_FindByScriptId(struct DirectionalManager *manager, s32 id, s32 *index)
{
    struct DirectionalController *e;
    s32 i;
    if (index != 0)
        *index = -1;
    e = manager->slots;
    for (i = 0; i < manager->count; i++, e++) {
        if (((1 << i) & manager->enabled) && e->id == id) {
            if (index != 0)
                *index = i;
            return e;
        }
    }
    return 0;
}

/* Picks the marker animation for a fade level. */
u32 DirectionalController_SetMarkerFadeLevel(struct DirectionalController *p, u32 level)
{
    void *animation = p->animation;
    if (p->disabled == 0)
        AnimData_SelectPaletteBank(animation, level + 0x6b);
}

/* Places the controller and its marker around the given point. */
u32 DirectionalController_SetPosition(struct DirectionalManager *manager,
                                      struct DirectionalController *p,
                                      struct DirectionalPackedVec *in)
{
    struct DirectionalDrawable *q = &p->drawable;
    p->position.x = in->x;
    p->position.y = in->y;
    p->position.z = in->z;
    q->position.x = in->x;
    q->position.y = in->y;
    q->position.z = in->z;
    if (p->orientation != 0) {
        p->position.x += 0x80;
        q->position.z -= 0x80;
    } else {
        p->position.z += 0x80;
        q->position.x -= 0x80;
    }
    q->position.x += 0x3c;
    q->position.z += 0x3c;
    q->position.y += 0x14;
}

void LevelController_Update(struct DirectionalManager *manager, struct DirectionalController *p)
{
    if (p->disabled != 1) {
        switch (p->state) {
            case 0:
                if (p->level == 0)
                    DirectionalController_SetMarkerFadeLevel(p, 2);
                else {
                    DirectionalController_SetMarkerFadeLevel(p, 1);
                    p->level--;
                }
                break;
            case 1:
                if (p->level > 4)
                    DirectionalController_SetMarkerFadeLevel(p, 0);
                else {
                    DirectionalController_SetMarkerFadeLevel(p, 1);
                    p->level++;
                }
                break;
            case 2:
                p->level = 5;
                DirectionalController_SetMarkerFadeLevel(p, 0);
                break;
        }
    }
}

s32 Actor_CanInteractWithController(struct DirectionalManager *manager,
                                    struct DirectionalController *p)
{
    struct Player *actor = manager->actor;
    u32 mask = 0x186c;
    u32 flags;
    flags = actor->statusFlags;
    if (flags & mask)
        return 0;
    flags &= 0x100000;
    if (!flags && manager->selected < 0)
        return 1;
    return 0;
}

void LevelController_RunScript(struct DirectionalManager *unused, struct DirectionalController *p,
                               u32 script)
{
    s32 values[5];
    struct DirectionalScriptArgs args;
    s32 i;
    if (script) {
        values[0] = p->id;
        for (i = 0; i < 4; i++)
            values[i + 1] = p->arguments[i];
        args.count = 5;
        args.values = values;
        Script_Call(script, &args);
    }
}

u32 DirectionalController_SelectTarget(struct DirectionalManager *manager, s32 index)
{
    manager->selected = index;
    manager->pending = 0;
}

u32 DirectionalController_ReleaseTarget(struct DirectionalManager *manager, s32 index)
{
    manager->selected = index;
    manager->pending = 1;
}

static inline s32 facingAllowed(u32 facing, s32 a, s32 b, s32 c)
{
    register u32 yes asm("r1") = 1;
    if (a == facing || b == facing || c == facing)
        return yes;
    return 0;
}

static inline s32 facingAllowedR2(u32 facing, s32 a, s32 b, s32 c)
{
    register u32 yes asm("r2") = 1;
    if (a == facing || b == facing || c == facing)
        return yes;
    return 0;
}

s32 DirectionalController_CheckApproach(struct DirectionalManager *manager,
                                        struct DirectionalController *p)
{
    DirectionalController_ConsumeEntered(p);
    if (manager->actor) {
        u32 mask = 6;
        u32 flags;
        flags = gUnk_02000560;
        if (flags & mask)
            goto done;
        flags &= 1;
        if (!flags)
            goto done;
        {
            s32 dx = p->position.x - manager->position.x, dz = p->position.z - manager->position.z;
            u32 distance = dx * dx + dz * dz;
            u32 direction = (((u16)Math_Atan2(dx, dz) + 112) >> 5) & 7;
            if (distance < p->distance) {
                s32 a, b, c;
                if (p->orientation) {
                    a = 0;
                    b = 1;
                    c = 2;
                } else {
                    a = 6;
                    b = 7;
                    c = 0;
                }
                if (a == direction || b == direction || c == direction) {
                    u32 facing = manager->actor->facing;
                    if (facingAllowed(facing, a, b, c))
                        DirectionalController_SetState(p, 1);
                }
            }
        }
    }
done:
    return 0;
}

s32 DirectionalController_CheckInput(struct DirectionalManager *manager,
                                     struct DirectionalController *p, s32 index)
{
    DirectionalController_ConsumeEntered(p);
    if (manager->actor) {
        u32 mask = 6;
        u32 flags;
        flags = gUnk_02000560;
        if (flags & mask)
            goto failed;
        flags &= 1;
        if (!flags)
            goto failed;
        {
            s32 dx = p->position.x - manager->position.x, dz = p->position.z - manager->position.z;
            u32 distance = dx * dx + dz * dz;
            u32 direction = (((u16)Math_Atan2(dx, dz) + 112) >> 5) & 7;
            if (distance < p->distance) {
                s32 a, b, c;
                if (p->orientation) {
                    a = 0;
                    b = 1;
                    c = 2;
                } else {
                    a = 6;
                    b = 7;
                    c = 0;
                }
                if (a == direction || b == direction || c == direction) {
                    u32 facing = manager->actor->facing;
                    if (facingAllowed(facing, a, b, c)) {
                        if ((gUnk_03005260[1] & 1) && Actor_CanInteractWithController(manager, p)) {
                            p->level = 5;
                            DirectionalController_SetMarkerFadeLevel(p, 0);
                            DirectionalController_SetState(p, 2);
                            DirectionalController_SelectTarget(manager, index);
                        }
                        goto done;
                    }
                }
            }
        }
    }
failed:
    DirectionalController_SetState(p, 0);
done:
    return 0;
}

s32 DirectionalController_CheckRemaining(struct DirectionalManager *manager,
                                         struct DirectionalController *p)
{
    DirectionalController_ConsumeEntered(p);
    if (manager->actor) {
        u32 mask = 6;
        u32 flags;
        flags = gUnk_02000560;
        if (flags & mask)
            goto done;
        flags &= 1;
        if (!flags)
            goto done;
        {
            s32 dx = p->position.x - manager->position.x, dz = p->position.z - manager->position.z;
            u32 distance = dx * dx + dz * dz;
            register s32 angle asm("r0") = Math_Atan2(dx, dz);
            u32 direction = ((({
                                  s32 copy = angle;
                                  asm("" : : "r"(copy));
                                  (u16) copy;
                              }) +
                              112) >>
                             5) &
                            7;
            if (distance < p->distance) {
                s32 a, b, c;
                if (p->orientation) {
                    a = 0;
                    b = 1;
                    c = 2;
                } else {
                    a = 6;
                    b = 7;
                    c = 0;
                }
                if (a == direction || b == direction || c == direction) {
                    u32 facing = manager->actor->facing;
                    if (facingAllowedR2(facing, a, b, c))
                        return angle;
                }
            }
        }
    }
done:
    DirectionalController_SetState(p, 0);
    return 0;
}

static inline u8 sameRegion(s32 region)
{
    if (gGameState->region == region)
        return 1;
    return 0;
}

s32 DirectionalController_UpdateAll(struct DirectionalManager *manager)
{
    struct DirectionalController *p;
    s32 i;
    if (manager->actor)
        manager->position = manager->actor->position;
    p = manager->slots;
    for (i = 0; i < manager->count; i++, p++) {
        if (manager->enabled & (1 << i)) {
            if (p->region >= 0 && !sameRegion(p->region)) {
                if (p->disabled != 1)
                    p->drawable.flags |= 1;
            } else {
                if (p->disabled != 1)
                    p->drawable.flags &= ~1;
                sub_08249244(manager, p, i, gUnk_086049F4[p->state]);
                LevelController_Update(manager, p);
            }
        }
    }
    if (manager->pending >= 0 && manager->selected >= 0) {
        p = &manager->slots[manager->selected];
        if (manager->pending == 0) {
            manager->pending = -1;
            LevelController_RunScript(manager, p, p->startScript);
        } else if (manager->pending == 1) {
            manager->pending = -1;
            manager->selected = -1;
            LevelController_RunScript(manager, p, p->endScript);
        }
    }
    return 0;
}

s32 DirectionalController_DestroyManager(struct DirectionalManager *manager)
{
    struct DirectionalController *p = manager->slots;
    s32 i;
    for (i = 0; i < manager->count; i++, p++)
        Sprite_Remove(&p->drawable);
    if (manager->slots)
        Heap_Free(manager->slots);
    gUnk_02000038 = 0;
    return 0;
}

s32 DirectionalController_InitManager(struct DirectionalManager *manager, void *unused)
{
    struct DirectionalController *p;
    s32 i;
    gUnk_02000038 = manager;
    manager->count = Script_GetKeywordValue('n', 1);
    manager->selected = -1;
    manager->pending = -1;
    manager->enabled = 0;
    manager->slots = Heap_Alloc(manager->count * sizeof(struct DirectionalController));
    if (!manager->slots) {
    fail:
        return -1;
    }
    Memory_Clear(manager->slots, manager->count * sizeof(struct DirectionalController));
    p = manager->slots;
    for (i = 0; i < manager->count; i++, p++)
        if (DirectionalController_InitSlot(manager, p, i) < 0)
            goto fail;
    manager->actor = gUnk_02000580[0];
    if (manager->actor)
        manager->position = manager->actor->position;
    else {
        manager->position.x = 0;
        manager->position.y = 0;
        manager->position.z = 0;
    }
    return 0;
}

void *DirectionalController_CreateManagerTask(void *a)
{
    struct DirectionalManager *p;
    if (gUnk_02000038 != 0)
        return gUnk_02000038;
    p = Task_Create(8, 0x30);
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void))DirectionalController_UpdateAll,
                          (void (*)(void))DirectionalController_DestroyManager);
        if (DirectionalController_InitManager(p, a) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

s32 DirectionalController_ScriptCreate(void)
{
    struct DirectionalPackedVec position;
    s32 findRegion;
    struct DirectionalManager *manager = gUnk_02000038;
    struct DirectionalController *p;
    s32 i;
    s32 id;
    if (!manager) {
        goto fail;
    }
    id = Script_GetKeywordValue('n', 0);
    p = manager->slots;
    for (i = 0; i < manager->count; i++, p++) {
        if (!(manager->enabled & (1 << i))) {
            u32 *drawable = &p->drawable.flags;
            p->id = id;
            p->orientation = Script_GetKeywordValue('d', 0);
            p->flags = 0;
            p->disabled = Script_GetKeywordValue('t', 0);
            findRegion = Script_GetKeywordValue('m', 0);
            p->startScript = Script_GetKeywordValue('b', 0);
            p->endScript = Script_GetKeywordValue('e', 0);
            {
                s32 distance = Script_GetKeywordValue('D', 230);
                p->distance = distance * distance;
            }
            if (Script_SeekToKeyword('a')) {
                s16 *base = p->arguments;
                s16 *out = *(&base);
                do {
                    s32 value = Script_GetPc();
                    if (value) {
                        *(&value) = Script_GetValue();
                    }
                    *out++ = value;
                } while ((s32)out <= (s32)(base + 3));
            } else {
                s16 *base = p->arguments;
                s32 zero = 0;
                s16 *out = p->arguments + 3;
                do {
                    *out-- = zero;
                } while ((s32)out >= (s32)base);
            }
            if (Script_SeekToKeyword('p')) {
                position.x = Script_GetValue();
                position.y = Script_GetValue();
                position.z = Script_GetValue();
            } else {
                position.x = 0;
                position.y = 0;
                position.z = 0;
            }
            DirectionalController_SetPosition(manager, p, &position);
            if (findRegion) {
                p->region = Region_FindContaining(&p->position);
            } else {
                p->region = -1;
            }
            DirectionalController_SetMarkerFadeLevel(p, 2);
            if (p->disabled == 1) {
                *drawable |= 1;
            } else {
                *drawable &= ~1;
            }
            if (p->orientation == 0) {
                *drawable &= ~4;
            } else {
                *drawable |= 4;
            }
            manager->enabled |= 1 << i;
            break;
        }
    }

    if (i != manager->count) {
        return 0;
    }
fail:
    return -1;
}

s32 DirectionalController_ScriptRelease(void)
{
    struct DirectionalManager *manager = gUnk_02000038;
    if (!manager || manager->selected < 0 || manager->selected >= manager->count)
        return -1;
    DirectionalController_ReleaseTarget(manager, manager->selected);
    DirectionalController_SetState(&manager->slots[manager->selected], 0);
    return 0;
}

s32 DirectionalController_ScriptMove(void)
{
    struct DirectionalPackedVec position;
    struct DirectionalManager *manager = gUnk_02000038;
    struct DirectionalController *p;
    if (!manager)
        goto fail;
    p = DirectionalController_FindByScriptId(manager, Script_GetKeywordValue('n', 0), 0);
    if (!p) {
    fail:
        return -1;
    }
    if (Script_SeekToKeyword('p')) {
        position.x = Script_GetValue();
        position.y = Script_GetValue();
        position.z = Script_GetValue();
    } else {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    DirectionalController_SetPosition(manager, p, &position);
    if (p->region >= 0)
        p->region = Region_FindContaining(&p->position);
}
