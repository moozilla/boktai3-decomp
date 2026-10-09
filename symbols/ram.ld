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
__divsi3 = sub_08249274;
_call_via_r1 = sub_0824923C;
_call_via_r2 = sub_08249240;
/* agbcc libgcc/lib1thumb.asm call_via r4: bx r4; nop (ROM 08249248). */
_call_via_r4 = sub_08249248;
/* Exact linked matches to pinned agbcc libgcc2 objects; see docs/LIBGCC.md. */
__lshrdi3 = sub_0824AC40;
__muldi3 = sub_0824AC74;
__negdi2 = sub_0824ACE4;

/* m4a_1.s routine whose label is only a data label in the generated asm */
MPlayMain = gUnk_0822F820 + 1;
gMaxLines = 0;

/* m4aSoundInit */
SoundMainRAM_Buffer = 0x030035F0;   /* 0x400 bytes: SoundMainRAM is copied here */
gSoundInfo = 0x030054E0;
gCgbChans = 0x03006520;
gMPlayMemAccArea = 0x030069E0;
SoundMainRAM = gUnk_0822F249;       /* ARM routine in ROM, copied to IWRAM (bit 0 cleared by the caller) */
