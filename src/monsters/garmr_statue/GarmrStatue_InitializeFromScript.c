#include "monsters/garmr_statue.h"
#include "player/cutscene/player_cue_sequence.h"
#include "engine/video/display_animation.h"
#include "engine/math/scene_vec.h"
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0820843C(void);
void sub_082086B8(void);
void GarmrStatue_InitCollider(void *);
void SceneBody_Init(void *, void *, void *, u32, u32, void *);
void SceneBody_SetSprite(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
s32 GarmrStatue_InitializeFromScript(struct GarmrStatue *p, void *parent)
{
    void *resource;
    u32 sequenceResult;
    if (Script_SeekToKeyword('p')) {
        p->initialPosition.x = Script_GetValue();
        p->initialPosition.y = Script_GetValue();
        p->initialPosition.z = Script_GetValue();
    }
    if (Script_SeekToKeyword('r')) {
        s32 r = Script_GetValue();
        p->triggerDistanceSquared = r * r;
    } else
        p->triggerDistanceSquared = 0x40000;
    if (Script_SeekToKeyword('a')) {
        p->effectA = Script_GetValue();
        p->effectB = Script_GetValue();
        p->effectC = Script_GetValue();
    } else {
        p->effectA = 30;
        p->effectB = 0;
        p->effectC = 30;
    }
    if (Script_SeekToKeyword('l')) {
        {
            s32 health = Script_GetValue();
            p->maxHealth = health;
            p->health = health;
            if ((s16)health < 0)
                p->defeatDelay = 1;
        }
    } else {
        p->maxHealth = 120;
        p->health = 120;
    }
    if (Script_SeekToKeyword('s'))
        p->projectileSpeed = Script_GetValue();
    else
        p->projectileSpeed = 12;
    if (Script_SeekToKeyword('S'))
        p->showTerrainBody = Script_GetValue();
    if (Script_SeekToKeyword('z')) {
        p->bodyReactionFlags = Script_GetValue();
        p->sparkMask = Script_GetValue();
    }
    if (Script_SeekToKeyword('g'))
        p->hudStyle = Script_GetValue();
    else
        p->hudStyle = 2;
    if (Script_SeekToKeyword('D'))
        p->unknown160 = Script_GetValue();
    if (Script_SeekToKeyword('G')) {
        p->unknown154 = Script_GetValue();
        p->unknown158 = Script_GetValue();
        p->unknown15C = Script_GetValue();
    }
    if (Script_SeekToKeyword('X')) {
        p->unknown140 = Script_GetValue();
        p->unknown142 = Script_GetValue();
    }
    if (Script_SeekToKeyword('V')) {
        p->hudTime = Script_GetValue();
        if (p->hudTime > 99)
            p->hudTime = 99;
        if (p->hudTime <= 0)
            p->hudTime = 1;
    } else
        p->hudTime = 0;
    p->hud = parent;
    AnimData_Load(p->graphics, 0x7e7d);
    Sprite_Init(&p->sprite.display, p->graphics, 1);
    p->sprite.coordinates.position = p->initialPosition;
    AnimData_SelectPaletteBank(p->graphics, 0x1bc);
    resource = ScriptProc_Start(0x922e, 0x5184);
    p->resource = resource;
    if (p->defeatDelay == 0) {
        p->update = sub_0820843C;
        sequenceResult = Anim_SetSequence(&p->sequence, resource, 8, 0, 0);
    } else {
        p->update = sub_082086B8;
        p->defeated = 1;
        sequenceResult =
            Anim_SetSequence(&p->sequence, p->resource, 10, 0, 0);
    }
    PlayerCue_StepSequenceEnded((struct ContactSwarmSprite *)(&p->sprite.display), (void *)(&p->sequence));
    if (p->defeatDelay == 0)
        GarmrStatue_InitCollider(p);
    if (p->showTerrainBody != 0) {
        u8 *body = p->terrainBody;
        SceneBody_Init(body, p->hud, &p->sprite.coordinates.position, 0, 4, p);
        SceneBody_SetSprite(body, &p->sprite.display);
        TerrainOverlay_QueueAddActor(body);
    }
#ifdef UBFIX
    // falls off the end with r0 left over from its last statement; the creator only tests
    // < 0, and this initializer has no failure path here
    return 0;
#endif
}
