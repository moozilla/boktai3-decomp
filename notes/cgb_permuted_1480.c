
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;
typedef volatile u8 vu8;
typedef volatile u16 vu16;
typedef volatile u32 vu32;
typedef volatile u64 vu64;
typedef volatile s8 vs8;
typedef volatile s16 vs16;
typedef volatile s32 vs32;
typedef volatile s64 vs64;
typedef float f32;
typedef double f64;
typedef u8 bool8;
typedef u16 bool16;
typedef u32 bool32;
typedef vu8 vbool8;
typedef vu16 vbool16;
typedef vu32 vbool32;
struct BgCnt
{
  u16 priority : 2;
  u16 charBaseBlock : 2;
  u16 dsCharBaseBlock : 2;
  u16 mosaic : 1;
  u16 palettes : 1;
  u16 screenBaseBlock : 5;
  u16 areaOverflowMode : 1;
  u16 screenSize : 2;
};
typedef volatile struct BgCnt vBgCnt;
struct PlttData
{
  u16 r : 5;
  u16 g : 5;
  u16 b : 5;
  u16 unused_15 : 1;
};
struct OamData
{
  u32 y : 8;
  u32 affineMode : 2;
  u32 objMode : 2;
  u32 mosaic : 1;
  u32 bpp : 1;
  u32 shape : 2;
  u32 x : 9;
  u32 matrixNum : 5;
  u32 size : 2;
  u16 tileNum : 10;
  u16 priority : 2;
  u16 paletteNum : 4;
  u16 affineParam;
};
struct BgAffineSrcData
{
  s32 texX;
  s32 texY;
  s16 scrX;
  s16 scrY;
  s16 sx;
  s16 sy;
  u16 alpha;
};
struct BgAffineDstData
{
  s16 pa;
  s16 pb;
  s16 pc;
  s16 pd;
  s32 dx;
  s32 dy;
};
struct ObjAffineSrcData
{
  s16 xScale;
  s16 yScale;
  u16 rotation;
};
struct SioMultiCnt
{
  u16 baudRate : 2;
  u16 si : 1;
  u16 sd : 1;
  u16 id : 2;
  u16 error : 1;
  u16 enable : 1;
  u16 unused_11_8 : 4;
  u16 mode : 2;
  u16 intrEnable : 1;
  u16 unused_15 : 1;
  u16 data;
};
void CpuSet(const void *src, void *dest, u32 control);
void CpuFastSet(const void *src, void *dest, u32 control);
struct MultiBootParam
{
  u32 system_work[5];
  u8 handshake_data;
  u16 handshake_timeout;
  u8 probe_count;
  u8 client_data[3];
  u8 palette_data;
  u8 response_bit;
  u8 client_bit;
  u8 reserved1;
  const u8 *boot_srcp;
  const u8 *boot_endp;
  const u8 *masterp;
  u8 *reserved2[3];
  u32 system_work2[4];
  u8 sendflag;
  u8 probe_target_bit;
  u8 check_wait;
  u8 server_type;
};
void SoftReset(u32 resetFlags);
void RegisterRamReset(u32 resetFlags);
void VBlankIntrWait(void);
u16 Sqrt(u32 num);
u16 ArcTan2(s16 x, s16 y);
void CpuSet(const void *src, void *dest, u32 control);
void CpuFastSet(const void *src, void *dest, u32 control);
void BgAffineSet(struct BgAffineSrcData *src, struct BgAffineDstData *dest, s32 count);
void ObjAffineSet(struct ObjAffineSrcData *src, void *dest, s32 count, s32 offset);
void LZ77UnCompWram(const u32 *src, void *dest);
void LZ77UnCompVram(const u32 *src, void *dest);
void RLUnCompWram(const u32 *src, void *dest);
void RLUnCompVram(const u32 *src, void *dest);
int MultiBoot(struct MultiBootParam *mp);
s32 Div(s32 num, s32 denom);
struct WaveData
{
  u16 type;
  u16 status;
  u32 freq;
  u32 loopStart;
  u32 size;
  s8 data[1];
};
struct ToneData
{
  u8 type;
  u8 key;
  u8 length;
  u8 pan_sweep;
  struct WaveData *wav;
  u8 attack;
  u8 decay;
  u8 sustain;
  u8 release;
};
struct CgbChannel
{
  u8 statusFlags;
  u8 type;
  u8 rightVolume;
  u8 leftVolume;
  u8 attack;
  u8 decay;
  u8 sustain;
  u8 release;
  u8 key;
  u8 envelopeVolume;
  u8 envelopeGoal;
  u8 envelopeCounter;
  u8 pseudoEchoVolume;
  u8 pseudoEchoLength;
  u8 dummy1;
  u8 dummy2;
  u8 gateTime;
  u8 midiKey;
  u8 velocity;
  u8 priority;
  u8 rhythmPan;
  u8 dummy3[3];
  u8 dummy5;
  u8 sustainGoal;
  u8 n4;
  u8 pan;
  u8 panMask;
  u8 modify;
  u8 length;
  u8 sweep;
  u32 frequency;
  u32 *wavePointer;
  u32 *currentPointer;
  struct MusicPlayerTrack *track;
  void *prevChannelPointer;
  void *nextChannelPointer;
  u8 dummy4[8];
};
struct MusicPlayerTrack;
struct SoundChannel
{
  u8 statusFlags;
  u8 type;
  u8 rightVolume;
  u8 leftVolume;
  u8 attack;
  u8 decay;
  u8 sustain;
  u8 release;
  u8 key;
  u8 envelopeVolume;
  u8 envelopeVolumeRight;
  u8 envelopeVolumeLeft;
  u8 pseudoEchoVolume;
  u8 pseudoEchoLength;
  u8 dummy1;
  u8 dummy2;
  u8 gateTime;
  u8 midiKey;
  u8 velocity;
  u8 priority;
  u8 rhythmPan;
  u8 dummy3[3];
  u32 count;
  u32 fw;
  u32 frequency;
  struct WaveData *wav;
  s8 *currentPointer;
  struct MusicPlayerTrack *track;
  void *prevChannelPointer;
  void *nextChannelPointer;
  u32 dummy4;
  u16 xpi;
  u16 xpc;
};
struct MusicPlayerInfo;
typedef void (*MPlayFunc)();
typedef void (*PlyNoteFunc)(u32, struct MusicPlayerInfo *, struct MusicPlayerTrack *);
typedef void (*CgbSoundFunc)(void);
typedef void (*CgbOscOffFunc)(u8);
typedef u32 (*MidiKeyToCgbFreqFunc)(u8, u8, u8);
typedef void (*ExtVolPitFunc)(void);
typedef void (*MPlayMainFunc)(struct MusicPlayerInfo *);
struct SoundInfo
{
  u32 ident;
  vu8 pcmDmaCounter;
  u8 reverb;
  u8 maxChans;
  u8 masterVolume;
  u8 freq;
  u8 mode;
  u8 c15;
  u8 pcmDmaPeriod;
  u8 maxLines;
  u8 gap[3];
  s32 pcmSamplesPerVBlank;
  s32 pcmFreq;
  s32 divFreq;
  struct CgbChannel *cgbChans;
  MPlayMainFunc MPlayMainHead;
  struct MusicPlayerInfo *musicPlayerHead;
  CgbSoundFunc sub_08230AC8;
  CgbOscOffFunc CgbOscOff;
  MidiKeyToCgbFreqFunc MidiKeyToCgbFreq;
  MPlayFunc *MPlayJumpTable;
  PlyNoteFunc plynote;
  ExtVolPitFunc ExtVolPit;
  u8 gap2[16];
  struct SoundChannel chans[12];
  s8 pcmBuffer[1584 * 2] __attribute__((aligned(4)));
};
struct SongHeader
{
  u8 trackCount;
  u8 blockCount;
  u8 priority;
  u8 reverb;
  struct ToneData *tone;
  u8 *part[1];
};
struct PokemonCrySong
{
  u8 trackCount;
  u8 blockCount;
  u8 priority;
  u8 reverb;
  struct ToneData *tone;
  u8 *part[2];
  u8 gap;
  u8 part0;
  u8 tuneValue;
  u8 gotoCmd;
  u32 gotoTarget;
  u8 part1;
  u8 tuneValue2;
  u8 cont[2];
  u8 volCmd;
  u8 volumeValue;
  u8 unkCmd0D[2];
  u32 unkCmd0DParam;
  u8 xreleCmd[2];
  u8 releaseValue;
  u8 panCmd;
  u8 panValue;
  u8 tieCmd;
  u8 tieKeyValue;
  u8 tieVelocityValue;
  u8 xwaitCmd[2];
  u16 length;
  u8 end[2];
};
struct MusicPlayerTrack
{
  u8 flags;
  u8 wait;
  u8 patternLevel;
  u8 repN;
  u8 gateTime;
  u8 key;
  u8 velocity;
  u8 runningStatus;
  u8 keyM;
  u8 pitM;
  s8 keyShift;
  s8 keyShiftX;
  s8 tune;
  u8 pitX;
  s8 bend;
  u8 bendRange;
  u8 volMR;
  u8 volML;
  u8 vol;
  u8 volX;
  s8 pan;
  s8 panX;
  s8 modM;
  u8 mod;
  u8 modT;
  u8 lfoSpeed;
  u8 lfoSpeedC;
  u8 lfoDelay;
  u8 lfoDelayC;
  u8 priority;
  u8 pseudoEchoVolume;
  u8 pseudoEchoLength;
  struct SoundChannel *chan;
  struct ToneData tone;
  u8 gap[10];
  u16 timer;
  u32 unk_3C;
  u8 *cmdPtr;
  u8 *patternStack[3];
};
struct MusicPlayerInfo
{
  struct SongHeader *songHeader;
  u32 status;
  u8 trackCount;
  u8 priority;
  u8 cmd;
  u8 unk_B;
  u32 clock;
  u8 gap[8];
  u8 *memAccArea;
  u16 tempoD;
  u16 tempoU;
  u16 tempoI;
  u16 tempoC;
  u16 fadeOI;
  u16 fadeOC;
  u16 fadeOV;
  struct MusicPlayerTrack *tracks;
  struct ToneData *tone;
  u32 ident;
  MPlayMainFunc MPlayMainNext;
  struct MusicPlayerInfo *musicPlayerNext;
};
struct MusicPlayer
{
  struct MusicPlayerInfo *info;
  struct MusicPlayerTrack *track;
  u8 numTracks;
  u16 unk_A;
};
struct Song
{
  struct SongHeader *header;
  u16 ms;
  u16 me;
};
extern const struct MusicPlayer gMPlayTable[];
extern const struct Song gSongTable[];
extern u8 gMPlayMemAccArea[];
extern struct PokemonCrySong gPokemonCrySong;
extern struct PokemonCrySong gPokemonCrySongs[];
extern struct MusicPlayerInfo gPokemonCryMusicPlayers[];
extern struct MusicPlayerTrack gPokemonCryTracks[];
extern char SoundMainRAM[];
extern MPlayFunc gMPlayJumpTable[];
typedef void (*XcmdFunc)(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
extern const XcmdFunc gXcmdTable[];
extern struct CgbChannel gCgbChans[];
extern const u8 gScaleTable[];
extern const u32 gFreqTable[];
extern const u16 gPcmSamplesPerVBlankTable[];
extern const u8 gCgbScaleTable[];
extern const s16 gCgbFreqTable[];
extern const u8 gNoiseTable[];
extern const struct PokemonCrySong gPokemonCrySongTemplate;
extern const struct ToneData voicegroup_dummy;
extern char gNumMusicPlayers[];
extern char gMaxLines[];
u32 sub_0822F1B4(u32 multiplier, u32 multiplicand);
void sub_0822F1C4(void);
void SoundMainBTM(void);
void sub_0822FA9C(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void MPlayMain(struct MusicPlayerInfo *);
void RealClearChain(void *x);
void MPlayContinue(struct MusicPlayerInfo *mplayInfo);
void sub_082306C4(struct MusicPlayerInfo *mplayInfo, struct SongHeader *songHeader);
void m4aMPlayStop(struct MusicPlayerInfo *mplayInfo);
void sub_082307E8(struct MusicPlayerInfo *mplayInfo);
void TrkVolPitSet(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void MPlayFadeOut(struct MusicPlayerInfo *mplayInfo, u16 speed);
void ClearChain(void *x);
void Clear64byte(void *addr);
void SoundInit(struct SoundInfo *soundInfo);
void MPlayExtender(struct CgbChannel *cgbChans);
void sub_082303EC(u32 mode);
void MPlayOpen(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *tracks, u8 trackCount);
void sub_08230AC8(void);
void CgbOscOff(u8);
void sub_08230A60(struct CgbChannel *chan);
u32 MidiKeyToCgbFreq(u8, u8, u8);
void sub_08231434(void);
void sub_0822F65C(MPlayFunc *mplayJumpTable);
void sub_08230380(u32 freq);
void sub_08230554(void);
void sub_082304D4(void);
void m4aMPlayTempoControl(struct MusicPlayerInfo *mplayInfo, u16 tempo);
void m4aMPlayVolumeControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u16 volume);
void m4aMPlayPitchControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s16 pitch);
void m4aMPlayPanpotControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s8 pan);
void ClearModM(struct MusicPlayerTrack *track);
void m4aMPlayModDepthSet(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u8 modDepth);
void m4aMPlayLFOSpeedSet(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u8 lfoSpeed);
struct MusicPlayerInfo *SetPokemonCryTone(struct ToneData *tone);
void SetPokemonCryVolume(u8 val);
void SetPokemonCryPanpot(s8 val);
void SetPokemonCryPitch(s16 val);
void SetPokemonCryLength(u16 val);
void SetPokemonCryRelease(u8 val);
void SetPokemonCryProgress(u32 val);
bool32 IsPokemonCryPlaying(struct MusicPlayerInfo *mplayInfo);
void SetPokemonCryChorus(s8 val);
void SetPokemonCryStereo(u32 val);
void SetPokemonCryPriority(u8 val);
void ply_fine(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_goto(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_patt(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_pend(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_rept(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_memacc(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_prio(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_tempo(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_keysh(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_voice(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_vol(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_pan(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_bend(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_bendr(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void sub_0822FD8C(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_lfodl(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void sub_0822FDA0(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_modt(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_tune(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_port(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xcmd(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void sub_0822FD18(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void m4a_Unk_0822FB18(u32 note_cmd, struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xxx(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xwave(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xtype(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xatta(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xdeca(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xsust(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xrele(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xiecv(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xiecl(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xleng(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xswee(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xwait(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
void ply_xcmd_0D(struct MusicPlayerInfo *, struct MusicPlayerTrack *);
extern const u8 gCgb3Vol[];
extern u8 SoundMainRAM_Buffer[0x400];
extern struct SoundInfo gSoundInfo;
u32 MidiKeyToFreq(struct WaveData *wav, u8 key, u8 fineAdjust);
void sub_0822FE18(void);
void MPlayContinue(struct MusicPlayerInfo *mplayInfo);
void MPlayFadeOut(struct MusicPlayerInfo *mplayInfo, u16 speed);
void m4aSoundInit(void);
void m4aSoundMain(void);
void m4aSongNumStart(u16 n);
void m4aSongNumStartOrChange(u16 n);
void m4aSongNumStartOrContinue(u16 n);
void m4aSongNumStop(u16 n);
void m4aSongNumContinue(u16 n);
void sub_08230018(void);
void m4aMPlayContinue(struct MusicPlayerInfo *mplayInfo);
void sub_08230050(void);
void m4aMPlayFadeOut(struct MusicPlayerInfo *mplayInfo, u16 speed);
void m4aMPlayFadeOutTemporarily(struct MusicPlayerInfo *mplayInfo, u16 speed);
void m4aMPlayFadeIn(struct MusicPlayerInfo *mplayInfo, u16 speed);
void sub_082300DC(struct MusicPlayerInfo *mplayInfo);
void MPlayExtender(struct CgbChannel *cgbChans);
void sub_0823025C(void);
void ClearChain(void *x);
void Clear64byte(void *x);
void SoundInit(struct SoundInfo *soundInfo);
void sub_08230380(u32 freq);
void sub_082303EC(u32 mode);
void SoundClear(void);
void sub_082304D4(void);
void sub_08230554(void);
void sub_082305CC(void);
void MPlayOpen(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *tracks, u8 trackCount);
void sub_082306C4(struct MusicPlayerInfo *mplayInfo, struct SongHeader *songHeader);
void m4aMPlayStop(struct MusicPlayerInfo *mplayInfo);
void sub_082307E8(struct MusicPlayerInfo *mplayInfo);
void TrkVolPitSet(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
u32 MidiKeyToCgbFreq(u8 chanNum, u8 key, u8 fineAdjust);
void CgbOscOff(u8 chanNum);
inline bool32 CgbPanInline(struct CgbChannel *chan);
void sub_08230A60(struct CgbChannel *chan);
void sub_08230AC8(void)
{
  s32 ch;
  struct CgbChannel *channels;
  s32 evAdd;
  s32 prevC15;
  struct SoundInfo *soundInfo = *((struct SoundInfo **) 0x3007FF0);
  vu8 *nrx0ptr;
  vu8 *nrx1ptr;
  vu8 *nrx2ptr;
  vu8 *nrx3ptr;
  vu8 *nrx4ptr;
  u8 chBit;
  u32 *new_var2;
  u8 *chBitp;
  int new_var;
  s32 chIdx;
  int mask = 0xff;
  if (soundInfo->c15)
  {
    soundInfo->c15--;
  }
  else
  {
    soundInfo->c15 = 14;
  }
  for (ch = 1, channels = soundInfo->cgbChans, chBitp = &chBit; ch <= 4; ch++, channels++)
  {
    if (!(channels->statusFlags & (((0x80 | 0x40) | 0x04) | 0x03)))
    {
      continue;
    }
    switch (ch)
    {
      case 1:
        nrx0ptr = (vu8 *) (0x4000000 + 0x60);
        nrx1ptr = (vu8 *) (0x4000000 + 0x62);
        nrx2ptr = (vu8 *) (0x4000000 + 0x63);
        nrx3ptr = (vu8 *) (0x4000000 + 0x64);
        nrx4ptr = (vu8 *) (0x4000000 + 0x65);
        chIdx = 0;
        break;

      case 2:
        nrx0ptr = (vu8 *) ((0x4000000 + 0x60) + 1);
        nrx2ptr = (vu8 *) (0x4000000 + 0x69);
        nrx1ptr = (vu8 *) (0x4000000 + 0x68);
        nrx3ptr = (vu8 *) (0x4000000 + 0x6c);
        nrx4ptr = (vu8 *) (0x4000000 - -0x6d);
        chIdx = 1;
        break;

      case 3:
        nrx0ptr = (vu8 *) (0x4000000 + 0x70);
        nrx1ptr = (vu8 *) (0x4000000 + 0x72);
        nrx2ptr = (vu8 *) (0x4000000 + 0x73);
        nrx3ptr = (vu8 *) (0x4000000 + 0x74);
        nrx4ptr = (vu8 *) (0x4000000 + 0x75);
        chIdx = 2;
        break;

      default:
        nrx0ptr = (vu8 *) ((0x4000000 + 0x70) + 1);
        nrx1ptr = (vu8 *) (0x4000000 + 0x78);
        nrx2ptr = (vu8 *) (0x4000000 + 0x79);
        nrx3ptr = (vu8 *) (0x4000000 + 0x7c);
        nrx4ptr = (vu8 *) (0x4000000 + 0x7d);
        chIdx = 3;
        break;

    }

    *chBitp = chIdx;
    prevC15 = soundInfo->c15;
    evAdd = *nrx2ptr;
    if (channels->statusFlags & 0x80)
    {
      if (!(channels->statusFlags & 0x40))
      {
        channels->statusFlags = 0x03;
        channels->modify = 0x02 | 0x01;
        sub_08230A60(channels);
        switch (ch)
        {
          case 1:
            *nrx0ptr = channels->sweep;

          case 2:
            *nrx1ptr = (((u32) channels->wavePointer) << 6) + channels->length;
            goto init_env_step_time_dir;

          case 3:
            if (channels->wavePointer != channels->currentPointer)
          {
            *nrx0ptr = 0x40;
            *((vu32 *) (0x4000000 + 0x90)) = channels->wavePointer[0];
            *((vu32 *) (0x4000000 + 0x94)) = channels->wavePointer[1];
            *((vu32 *) (0x4000000 + 0x98)) = channels->wavePointer[2];
            *((vu32 *) (0x4000000 + 0x9c)) = channels->wavePointer[3];
            new_var2 = channels->wavePointer;
            channels->currentPointer = new_var2;
          }
            *nrx0ptr = 0;
            *nrx1ptr = channels->length;
            if (channels->length)
          {
            channels->n4 = 0xC0;
          }
          else
          {
            int v = -128;
            channels->n4 = v;
          }
            break;

          default:
            *nrx1ptr = channels->length;
            *nrx3ptr = ((u32) new_var2) << 3;
            init_env_step_time_dir:
          evAdd = channels->attack + 0x08;

            if (channels->length)
          {
            channels->n4 = 0x40;
          }
          else
          {
            channels->n4 = 0x00;
          }
            break;

        }

        channels->envelopeCounter = channels->attack;
        if ((s8) (channels->attack & mask))
        {
          channels->envelopeVolume = 0;
          goto envelope_step_complete;
        }
        else
        {
          goto envelope_decay_start;
        }
      }
      else
      {
        goto oscillator_off;
      }
    }
    else
      if ((channels->statusFlags & 0x04) || (!(((*((vu8 *) (0x4000000 + 0x84))) >> (*chBitp)) & 1)))
    {
      channels->pseudoEchoLength--;
      if (((s8) (channels->pseudoEchoLength & mask)) <= 0)
      {
        oscillator_off:
        CgbOscOff(ch);

        channels->statusFlags = 0;
        goto channel_complete;
      }
      goto envelope_complete;
    }
    else
      if ((channels->statusFlags & 0x40) && (channels->statusFlags & 0x03))
    {
      channels->statusFlags &= ~0x03;
      channels->envelopeCounter = channels->release;
      if ((s8) (channels->release & mask))
      {
        channels->modify |= 0x01;
        if (ch != 3)
        {
          evAdd = channels->release | 0x00;
        }
        goto envelope_step_complete;
      }
      else
      {
        goto envelope_pseudoecho_start;
      }
    }
    else
    {
      envelope_step_repeat:
      if (channels->envelopeCounter == 0)
      {
        if (ch == 3)
        {
          channels->modify |= 0x01;
        }
        sub_08230A60(channels);
        if ((channels->statusFlags & 0x03) == 0x00)
        {
          channels->envelopeVolume--;
          if (((s8) (channels->envelopeVolume & mask)) <= 0)
          {
            envelope_pseudoecho_start:
            channels->envelopeVolume = ((channels->envelopeGoal * channels->pseudoEchoVolume) + 0xFF) >> 8;

            if (channels->envelopeVolume)
            {
              channels->statusFlags |= 0x04;
              channels->modify |= 0x01;
              if (ch != 3)
              {
                evAdd = 0 | 0x08;
              }
              goto envelope_complete;
            }
            else
            {
              goto oscillator_off;
            }
          }
          else
          {
            channels->envelopeCounter = channels->release;
          }
        }
        else
          if ((channels->statusFlags & 0x03) == 0x01)
        {
          envelope_sustain:
          channels->envelopeVolume = channels->sustainGoal;

          channels->envelopeCounter = 7;
        }
        else
          if ((channels->statusFlags & 0x03) == 0x02)
        {
          int envelopeVolume;
          int sustainGoal;
          channels->envelopeVolume--;
          envelopeVolume = (s8) (channels->envelopeVolume & mask);
          sustainGoal = (s8) channels->sustainGoal;
          new_var = 0x01;
          if (envelopeVolume <= sustainGoal)
          {
            envelope_sustain_start:
            if (channels->sustain == 0)
            {
              channels->statusFlags &= ~0x03;
              goto envelope_pseudoecho_start;
            }
            else
            {
              channels->statusFlags--;
              channels->modify |= new_var;
              if (ch != 3)
              {
                evAdd = 0 | 0x08;
              }
              goto envelope_sustain;
            }

          }
          else
          {
            channels->envelopeCounter = channels->decay;
          }
        }
        else
        {
          channels->envelopeVolume++;
          if (((u8) (channels->envelopeVolume & mask)) >= channels->envelopeGoal)
          {
            envelope_decay_start:
            channels->statusFlags--;

            channels->envelopeCounter = channels->decay;
            if ((u8) (channels->envelopeCounter & mask))
            {
              channels->modify |= new_var;
              channels->envelopeVolume = channels->envelopeGoal;
              if (ch != 3)
              {
                evAdd = channels->decay | 0x00;
              }
            }
            else
            {
              goto envelope_sustain_start;
            }
          }
          else
          {
            channels->envelopeCounter = channels->attack;
          }
        }
      }

    }
    envelope_step_complete:
    channels->envelopeCounter--;

    if (prevC15 == 0)
    {
      prevC15--;
      goto envelope_step_repeat;
    }
    envelope_complete:
    if (channels->modify & 0x02)
    {
      if ((ch < 4) && (channels->type & 0x08))
      {
        int dac_pwm_rate = *((vu8 *) (0x4000000 + 0x89));
        if (dac_pwm_rate < 0x40)
        {
          channels->frequency = (channels->frequency + 2) & 0x7fc;
        }
        else
          if (dac_pwm_rate < 0x80)
        {
          channels->frequency = (channels->frequency + 1) & 0x7fe;
        }
      }
      if (ch != 4)
      {
        *nrx3ptr = channels->frequency;
      }
      else
      {
        *nrx3ptr = ((*nrx3ptr) & 0x08) | channels->frequency;
      }
      channels->n4 = (channels->n4 & 0xC0) + ((channels->frequency & 0x3F00) >> 8);
      *nrx4ptr = (s8) (channels->n4 & mask);
    }

    if (channels->modify & new_var)
    {
      *((vu8 *) (0x4000000 + 0x81)) = ((*((vu8 *) (0x4000000 + 0x81))) & (~channels->panMask)) | channels->pan;
      if (ch == 3)
      {
        *nrx2ptr = gCgb3Vol[channels->envelopeVolume];
        if (channels->n4 & 0x80)
        {
          *nrx0ptr = 0x80;
          *nrx4ptr = channels->n4;
          channels->n4 &= 0x7f;
        }
      }
      else
      {
        evAdd &= 0xf;
        *nrx2ptr = (channels->envelopeVolume << 4) + evAdd;
        *nrx4ptr = channels->n4 | 0x80;
        if ((ch == 1) && (!((*nrx0ptr) & 0x08)))
        {
          *nrx4ptr = channels->n4 | 0x80;
        }
      }
    }
    channel_complete:
    channels->modify = 0;

  }

}

void m4aMPlayTempoControl(struct MusicPlayerInfo *mplayInfo, u16 tempo);
void m4aMPlayVolumeControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u16 volume);
void m4aMPlayPitchControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s16 pitch);
void m4aMPlayPanpotControl(struct MusicPlayerInfo *mplayInfo, u16 trackBits, s8 pan);
void ClearModM(struct MusicPlayerTrack *track);
void m4aMPlayModDepthSet(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u8 modDepth);
void m4aMPlayLFOSpeedSet(struct MusicPlayerInfo *mplayInfo, u16 trackBits, u8 lfoSpeed);
void ply_memacc(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xcmd(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xxx(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xwave(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xtype(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xatta(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xdeca(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xsust(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xrele(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xiecv(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xiecl(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xleng(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
void ply_xswee(struct MusicPlayerInfo *mplayInfo, struct MusicPlayerTrack *track);
