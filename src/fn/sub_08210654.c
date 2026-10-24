// MATCHING_CONCESSION: register-pinning
#include "global.h"
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: option/animation register allocation and defined success return.
#include "pink_spine_playback.h"
// confidence=low: prefix follows the neighboring creator; species identity remains tentative.
void ProjectileEmitter_WaitForPlayerApproach(void);
void ProjectileEmitter_EmitAttackFrame(void);
s32 PinkSpine_InitCollider(struct PinkSpineState *);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
s32 PinkSpine_InitFromScript(struct PinkSpineState *p, void *resource)
{
    void *graphics;
    void *sprite;
    void *initialSprite;
    void *sequences;
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        p->nearRadiusSquared = Script_GetValue();
        p->farRadiusSquared = Script_GetValue();
        p->nearRadiusSquared *= p->nearRadiusSquared;
        p->farRadiusSquared *= p->farRadiusSquared;
    } else {
        p->nearRadiusSquared = 0x90000;
        p->farRadiusSquared = 0x10000;
    }
    if (Script_SeekToKeyword('l')) {
        p->f136 = Script_GetValue();
        if (p->f136 < 0)
            p->f13A = 1;
    }
    if (Script_SeekToKeyword('a')) {
        p->f124 = Script_GetValue();
        p->f126 = Script_GetValue();
        p->f128 = Script_GetValue();
        p->f12E = Script_GetValue();
    } else {
        p->f124 = 30;
        p->f126 = 0;
        p->f128 = 30;
        p->f12E = 24;
    }
    if (Script_SeekToKeyword('S'))
        p->f138 = Script_GetValue();
    if (Script_SeekToKeyword('D'))
        p->f158 = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->f14C = Script_GetValue();
        p->f150 = Script_GetValue();
        p->f154 = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->f13C = Script_GetValue();
        p->f13E = Script_GetValue();
    }
    p->f140 = resource;
    graphics = p->graphics;
    AnimData_Load(graphics, 0x7E7D);
    initialSprite = &p->sprite;
    Sprite_Init(initialSprite, graphics, 1);
    p->sprite.position = p->initialPosition;
    AnimData_SelectPaletteBank(graphics, 0x1BD);
    sequences = ScriptProc_Start(0x922E, 0x5184);
    p->f15C = sequences;
    sprite = initialSprite;
    asm("" : "+r"(initialSprite));
    if (!p->f13A) {
        p->update = ProjectileEmitter_WaitForPlayerApproach;
        Anim_SetSequence(&p->sequence, sequences, 0xB, 0, 0);
    } else {
        p->update = ProjectileEmitter_EmitAttackFrame;
        p->f134 = 1;
        Anim_SetSequence(&p->sequence, p->f15C, 0xD, 0, 0);
    }
    {
        register u32 allocationToken asm("r0");
        asm("" : "=r"(allocationToken));
        PinkSpine_StepAnimation(p);
        asm("" : : "r"(allocationToken));
    }
    if (!p->f13A)
        PinkSpine_InitCollider(p);
    if (p->f138) {
        SceneBody_Init(p->body, p->f140, &p->sprite.position, 0, 4, p);
        SceneBody_SetSprite(p->body, sprite);
        TerrainOverlay_QueueAddActor(p->body);
    }
    p->f136 = 1;
    return 0;
}

#else
INCLUDE_ASM("asm/nonmatching", PinkSpine_InitFromScript);
#endif
