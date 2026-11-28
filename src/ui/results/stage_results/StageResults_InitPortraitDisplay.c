#include "global.h"
#include "ui/results/stage_results.h"

struct PortraitPosition {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 unused : 16;
};

void SceneActorStyle_InitializeFromGameState(void *);
void ActorSprite_InitPartDrawings(void *, void *, u32);
void CharacterPreview_PositionParts(void *, void *, void *, s32);
void CharacterModel_SetAnimationFlags(void *, s32);
void CharacterModel_SetComponentRenderMode(void *, s32);
void CharacterModel_InitPalettes(void *, u32);

void StageResults_InitPortraitDisplay(struct StageResults *results)
{
    struct PortraitPosition position;
    SceneActorStyle_InitializeFromGameState(results->portraitStyle);
    results->unk11B2 = 0;
    position.x = 0x30;
    position.y = 0x50;
    position.z = 0;
    ActorSprite_InitPartDrawings(results->portrait, results->model, 0x3D99);
    CharacterPreview_PositionParts(results->portrait, results->model, &position, 0);
    CharacterModel_SetAnimationFlags(results->model, 0);
    CharacterModel_SetComponentRenderMode(results->model, 0);
    CharacterModel_InitPalettes(results->model, results->portraitStyle[3]);
}
