#ifndef GUARD_SCENE_PANEL_H
#define GUARD_SCENE_PANEL_H
#include "engine.h"
#include "background_controller.h"
#include "contact_actor.h"
#include "scene_object.h"
#include "contact_swarm.h"
#include "render.h"
struct ScenePanel;
struct ScenePanelInteractionPlayer;
struct TetherActor;

/* Subject identity remains tentative; fields follow observed consumers. */
struct ScenePanelSubject {
    u8 unknown0[0x18];
    struct ContactSwarmSprite drawable;
    u8 graphics[0x1C];
    struct ContactPosition home;
    struct ContactCollider collider;
    struct DisplaySequenceState sequence;
    void *sequenceResource;
    u8 sequenceRow, sequenceVariant, kind, state;
    u16 stateFrame, contactProgress, contactLimit;
    u8 unknownDA[2];
    u16 progressStep;
    u16 timer, appearDelay, unknownE2, contactCooldown, requested;
    u16 requestLevel, unknownEA, hold;
    u8 unknownEE, contactKind, changed, positionReset;
    u16 paletteBank, basePaletteBank, countdown, nextThreshold;
    u8 unknownFA[3], byteTimer;
    u8 unknownFE[6];
    u32 effects[0x138 / 4];
    void (*update)(struct ScenePanelSubject *);
};
/* Fifteen-sprite rig embedded at parent offset 0xC6C. */
struct SceneRigSprite {
    u32 flags;
    u8 unknown4[3];
    u8 renderMode;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
    u8 graphics[0x1C];
    struct DisplaySequenceState sequence;
    void *animationResource;
    u8 unknown5C[8];
    u16 randomPhase, phase;
};
struct ScenePanelRig {
    struct SceneRigSprite main, primary[8], secondary[6];
    struct ContactPosition position;
    struct ContactCollider collider;
    struct ContactPosition origin;
    u16 state, stateFrame, angle;
    s16 turnStep;
    s16 turnRate;
    u16 turnDuration;
    u16 distance, speed;
    u16 verticalStep, colliderHold;
    u8 motionFlag;
    u8 contactDirection;
    u16 contactForce, contactEffect;
    s8 selectedSubject;
    u8 unknown697[3];
    u16 resourceAccumulator, selectionTimer;
    u8 unknown69E[2];
    u8 payloadA[8], payloadB[8];
    struct ScenePanel *parent;
};
struct ScenePanelAuxSprite {
    u32 flags;
    u8 unknown4[3];
    u8 renderMode, parameter8, parameter9;
    u8 unknownA[6];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
    u8 graphics[0x1C];
};
struct ScenePanelPaletteGraphics {
    u8 unknown0[0xC];
    u16 *palette;
    u8 unknown10[0xC];
};
struct ScenePanelPaletteSprite {
    u32 flags;
    u8 unknown4[3];
    u8 renderMode;
    u8 unknown8[8];
    u16 frame;
    u8 unknown12[0xA];
    struct ContactPosition position;
    u8 unknown24[8];
    struct ScenePanelPaletteGraphics graphics;
    u16 paletteCopy[16];
    u16 active, transform[3];
};
struct ScenePanelCommand {
    u8 unknown0[6];
    u8 opcode, unknown7;
    u16 mode;
};
struct ScenePanelReceiver {
    u16 actorId;
    u8 category, pendingTrigger;
    u8 unknown4[2], count[2];
    struct ScenePanelCommand *commands[2][4];
};
/* Shared parent layout from presentation, interaction, configuration and lifecycle consumers.
 * Presentation mode and effect state retain the original public field names. */
struct ScenePanel {
    struct Task task;
    u16 actorId, initialMode;
    u8 unknown1C[2];
    u8 subjectCount;
    u8 activeCount;
    u8 unknown20[0x20E];
    u16 stateFrame, channels[8];
    u8 unknown240[0x10];
    void (*state)(struct ScenePanel *);
    struct ScenePanelPaletteSprite paletteSprite;
    u8 unknown2C4[0x210];
    void *backgroundMap, *palette;
    void *overlayMap, *overlayPalette;
    u8 targetLevels[4], currentLevels[4];
    u16 overlayFrame;
    u8 unknown4EE[6];
    struct ScenePanelInteractionPlayer *savedPlayer;
    struct TetherActor *savedTether;
    u16 effectState, effectFrame;
    struct ContactPosition origin;
    struct ContactPosition approachPosition;
    /* Signed vector and packed presentation arithmetic share these eight bytes. */
    struct ContactPosition center;
    struct ScenePanelAuxSprite auxiliary;
    u8 unknown560[0x688];
    struct ScenePanelSubject *subjects[4];
    u16 resourceScale;
    u8 unknownBFA[0x15];
    u8 skipMainSprite;
    u8 paletteMode, savedMode, gateC12;
    u8 unknownC13[3];
    u16 variant;
    u8 unknownC18[0x54];
    struct ScenePanelRig rig;
    u8 unknown1320[8];
    u16 mode;
    s16 shakeOffset;
    u8 unknown132C[2];
    u8 overlayChanged;
    u8 unknown132F[7];
    u8 allSubjectsNotified, arrivalRampEnabled;
    u16 arrivalRampFrame;
    u8 resourceIndex;
    u8 unknown133B[7];
    u8 activated, warningStage;
    u16 warningSounds[4];
    u8 unknown134C[6];
    u16 resourceId;
    u8 unknown1354[0x24];
    u16 effectPhase;
    u8 unknown137A[0x4A];
    u16 parameter;
    u8 unknown13C6[0x62];
    u32 transitionComplete;
    u8 unknown142C[4];
    u16 channelsDirty;
    u8 unknown1432[0x37A];
    s32 property;
    struct ScenePanelReceiver receiver;
    u8 unknown17D8[0xC];
};
extern const u16 gUnk_08605A88[];
void ScenePanel_SetChannelValue(struct ScenePanel *, s32, u16);
void ScenePanel_SetState(struct ScenePanel *, void (*)(struct ScenePanel *));
s32 ScenePanel_GetPaletteOffset(struct ScenePanel *);
void sub_08058168(struct ScenePanel *);
void sub_08058184(struct ScenePanel *);
void ScenePanel_BeginForwardSequence(struct ScenePanel *);
void sub_0805822C(struct ScenePanel *);
void ScenePanel_BeginLongSequence(struct ScenePanel *);
void ScenePanel_BeginPulseSequence(struct ScenePanel *);
void ScenePanel_LoadModePalette(struct ScenePanel *);
void ScenePanel_SetPresentationMode(struct ScenePanel *);
void ScenePanel_TickForwardThree(struct ScenePanel *);
void ScenePanel_TickForwardFour(struct ScenePanel *);
void ScenePanel_TickReverseFour(struct ScenePanel *);
void ScenePanel_TickReverseThree(struct ScenePanel *);
void ScenePanel_TickLongSix(struct ScenePanel *);
void ScenePanel_TickLongSeven(struct ScenePanel *);
void ScenePanel_TickLongEight(struct ScenePanel *);
void ScenePanel_TickLongNine(struct ScenePanel *);
void ScenePanel_TickLongTen(struct ScenePanel *);
void ScenePanel_TickLongEleven(struct ScenePanel *);
void ScenePanel_TickPulseRest(struct ScenePanel *);
void ScenePanel_TickPulseTwelve(struct ScenePanel *);
void ScenePanel_TickPulseThirteen(struct ScenePanel *);
void ScenePanel_TickPulseTwo(struct ScenePanel *);
/* 0x020004A4 */
extern struct ScenePanel *gScenePanel;
void ScenePanel_CallPendingScript(struct ScenePanel *, u32);
union ScenePanelPosition;
void ScenePanel_AddParticle(struct ScenePanel *, union ScenePanelPosition *,
                            union ScenePanelPosition *, u32, u32);
void SparkParticle_Spawn(const void *, s32, const void *, s32, s32, s32, s32, s32);
#endif
