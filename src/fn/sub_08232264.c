#include "global.h"
struct EffectManager {
    u8 task[0x18];
    void *animData;
    void *sequence;
    void *head;
};
struct EffectGroupEntry {
    u8 state, fadeTimer;
    u16 frame;
    u8 unknown4[4];
    u16 x, y, z;
    u8 unknownE[2];
    u8 sprite[0x28];
    u8 anim[0x10];
};
struct EffectGroup {
    u8 active, mode, cursor, count;
    u8 kind, variant;
    u8 unknown6[2];
    void *owner;
    struct EffectGroupEntry entries[4];
    void (*update)(struct EffectGroup *);
};
extern struct EffectManager *gSceneActorEffectsManager;
struct EffectManager *SceneActorEffects_CreateManager(void *, void *);
void ParticleSprite_Initialize(void *, void *, u32);
void Effect_SetOrigin(void *, s32, s32);
void Anim_SetSequence(void *, void *, u32, u32, u32);
s32 ActiveList_PrependNode(struct EffectManager *, struct EffectGroup *);
void sub_08231444(struct EffectGroup *);
// confidence=low: sets up an effect group of four particles for an owner and adds it to the
// scene actor effect manager, creating the manager if needed.
s32 sub_08232264(struct EffectGroup *group, void *owner, s32 mode, s32 kind, s32 variant)
{
    struct EffectManager *manager = gSceneActorEffectsManager;
    struct EffectGroupEntry *entry;
    s32 i;
    if (manager == NULL) {
        manager = SceneActorEffects_CreateManager(NULL, NULL);
        if (manager == NULL)
            return -1;
    }
    group->active = 0;
    group->owner = owner;
    group->cursor = 0;
    group->count = 0;
    group->mode = mode;
    group->kind = kind;
    group->variant = variant;
    group->update = sub_08231444;
    for (i = 0; i < 4; i++) {
        entry = &group->entries[i];
        entry->state = 0;
        entry->frame = 0;
        entry->x = 0;
        entry->y = 0;
        entry->z = 0;
        ParticleSprite_Initialize(entry->sprite, gSceneActorEffectsManager->animData, 0x11);
        Effect_SetOrigin(entry->sprite, -8, -8);
        entry->sprite[0xF] = 2;
        Anim_SetSequence(entry->anim, gSceneActorEffectsManager->sequence, 0, 0, 0);
    }
    ActiveList_PrependNode(manager, group);
    return 0;
}
