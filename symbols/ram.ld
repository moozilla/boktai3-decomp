/* Symbols that C units reference by name but that are not defined as labels in
 * the generated asm. Appended to the link script by tools/build.py.
 * Names follow pret/pokeemerald src/m4a.c; addresses derived from the literal
 * pools of the matching functions. */

/* RAM */
gMPlayJumpTable = 0x03006490;   /* 36 x MPlayFunc, 0x90 bytes */

/* constants the SDK links in as absolute symbols */
gNumMusicPlayers = 0x20;        /* NUM_MUSIC_PLAYERS */

/* ROM data (use labels so the data stays movable) */
gScaleTable = gUnk_0825006C;
gFreqTable = gUnk_08250120;
gPcmSamplesPerVBlankTable = gUnk_08250150;
gCgbScaleTable = gUnk_08250168;
gCgbFreqTable = gUnk_082501EC;
gNoiseTable = gUnk_08250204;
gCgb3Vol = gUnk_08250240;
gXcmdTable = gUnk_08250284;
gMPlayTable = gUnk_082643EC;

/* libgcc/AGB SDK helpers emitted by the compiler */
_call_via_r1 = Callback_InvokeActor;
_call_via_r2 = sub_08249240;
/* agbcc libgcc/lib1thumb.asm call_via r4: bx r4; nop (ROM 08249248). */
_call_via_r4 = sub_08249248;
/* lib1funcs __modsi3 (ROM 08249350), emitted for signed %. */
__modsi3 = sub_08249350;
/* Exact linked matches to pinned agbcc libgcc2 objects; see docs/LIBGCC.md. */
__lshrdi3 = Libgcc_ShiftRightLogical64;
__muldi3 = Libgcc_MultiplyInt64;
__negdi2 = Libgcc_NegateInt64;

/* m4a_1.s routine whose label is only a data label in the generated asm */
MPlayMain = gUnk_0822F820 + 1;
gMaxLines = 0;

/* m4aSoundInit */
SoundMainRAM_Buffer = 0x030035F0;   /* 0x400 bytes: SoundMainRAM is copied here */
gSoundInfo = 0x030054E0;
gCgbChans = 0x03006520;
gMPlayMemAccArea = 0x030069E0;
SoundMainRAM = gUnk_0822F249;       /* ARM routine in ROM, copied to IWRAM (bit 0 cleared by the caller) */

/* Fully linked newlib math calls verify these libgcc helper aliases; docs/LIBM.md. */
/* Exact libgcc source unit identifies comparison helper; docs/LIBGCC.md. */
/* Exact old_agbcc libgcc2 source and lib1thumb division identify these helpers. */
__fixunsdfsi = Libgcc_ConvertDoubleToUint32;
__udivsi3 = sub_08249420;

/* agbcc indirect-call helper: original 08249244 is bx r3; nop. */
_call_via_r3 = sub_08249244;
/* agbcc indirect-call trampoline: bx r5; nop. */
_call_via_r5 = sub_0824924C;
