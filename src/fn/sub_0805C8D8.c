#include "scene_panel_lifecycle.h"
#define ScenePanel_TickLifecycle sub_0805C8D8
s32 sub_0805C8D8(struct ScenePanelLifecycleView *parent){
 u32 playerKind;
 sub_0805C680((void*)parent);sub_0805C774((void*)parent);parent->activeCount=sub_0805C7B4((void*)parent);
 playerKind=SCENE_PANEL_PLAYER->kindValue;
 if(playerKind==4)return 0;
 if(parent->initialMode==1 && parent->state!=14 && playerKind==1 && !gUnk_02000710->unk618){
 ScenePanel_SetEffectState((void*)parent,14);ScenePanelRig_SetState(&parent->rig,9);ScenePanel_NoOpAction((void*)parent);ScenePanel_CallPendingScript((void*)parent,8);
 }
 gUnk_08605ABC[parent->state]((void*)parent);
 if(parent->initialMode==0)ScenePanel_UpdatePaletteState((void*)parent);
 else{
 ScenePanel_TickRig((void*)parent);sub_0805C86C((void*)parent);ScenePanel_DrawBackgroundLayers((void*)parent);ScenePanel_LoadModePalette((void*)parent);sub_0805C7E4((void*)parent);
 if(parent->activated){
 if(!parent->skipMainSprite){
 if(gUnk_02000710->unk618==0){if(parent->warningSounds[2])Sound_PlaySong(parent->warningSounds[2]);}
 else{if(parent->warningSounds[parent->warningStage])Sound_PlaySong(parent->warningSounds[parent->warningStage]);}
 Sound_PlaySong(0x29B);
 }
 parent->activated=0;
 }
 }
 /* The non-void callback preserves the original fall-through return ABI. */
 ScenePanel_UpdateParticles((void*)parent);
}
