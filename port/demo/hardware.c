/* Browser hardware showcase. The game's full sequencer remains in src/;
 * this bounded PCM sequencer is only a runtime-ROM mixer demonstration. */
#include "hw.h"
#include "gba/m4a_internal.h"
#include "../hw/internal.h"

static uint8_t rom[0x2000000];
static uint32_t rom_size;
static Hw hardware;
static struct SoundInfo sound;
static int16_t pcm[352 * 2];
static uint32_t voicegroup;
static unsigned tempo, accumulator, track_count, note_count, skipped, failed, mode;
static const uint8_t clocks[49] = {
    0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,
    28,30,32,36,40,42,44,48,52,54,56,60,64,66,68,72,76,78,80,84,88,90,92,96
};
typedef struct DemoTrack {
    uint32_t pointer, stack[3];
    unsigned wait, depth, repeat, program, running, key, velocity, volume, bend_range;
    int keyshift, pan, bend, tune, on;
} DemoTrack;
typedef struct DemoVoice {
    DemoTrack *track;
    unsigned gate, key;
    int pitch_key, pan;
} DemoVoice;
static DemoTrack tracks[16];
static DemoVoice voices[12];

uint8_t *demo_rom_buffer(void) { return rom; }
uint32_t demo_rom_capacity(void) { return sizeof(rom); }
int16_t *demo_pcm_buffer(void) { return pcm; }
unsigned demo_notes(void) { return note_count; }
unsigned demo_skipped(void) { return skipped; }
unsigned demo_error(void) { return failed; }
void demo_load_rom(uint32_t size) { rom_size = size <= sizeof(rom) ? size : 0; }

static uint8_t byte(uint32_t address)
{
    if (address < 0x08000000 || address - 0x08000000 >= rom_size) { failed = 1; return 0; }
    return rom[address - 0x08000000];
}
static uint32_t word(uint32_t a) { return (uint32_t)byte(a) | (uint32_t)byte(a+1)<<8 | (uint32_t)byte(a+2)<<16 | (uint32_t)byte(a+3)<<24; }

static uint32_t pitch(uint32_t base, int semitones256)
{
    static const uint32_t ratio[13] = {65536,69433,73561,77936,82570,87480,92682,98193,104032,110218,116772,123715,131072};
    int octave = semitones256 / (12 * 256), remainder = semitones256 % (12 * 256);
    if (remainder < 0) { remainder += 12 * 256; --octave; }
    unsigned key = (unsigned)remainder >> 8, fraction = (unsigned)remainder & 255;
    uint64_t rate = (uint64_t)base * (ratio[key] + ((ratio[key+1] - ratio[key]) * fraction >> 8)) >> 16;
    if (octave > 12) return 0;
    if (octave < -24) return 0;
    rate = octave >= 0 ? rate << octave : rate >> -octave;
    return rate > 0xffffffff ? 0xffffffff : (uint32_t)rate;
}

static void release(DemoTrack *t, int key)
{
    unsigned i;
    for (i=0; i<12; ++i) if (voices[i].track == t && (key < 0 || voices[i].key == (unsigned)key)) sound.chans[i].statusFlags |= 0x40;
}

static void note(DemoTrack *t, unsigned command)
{
    unsigned gate = clocks[command - 0xcf], i, chosen = 12;
    uint32_t tone = voicegroup + t->program * 12;
    int pitch_key, pan = 0;
    if (byte(t->pointer) < 0x80) {
        t->key = byte(t->pointer++);
        if (byte(t->pointer) < 0x80) {
            t->velocity = byte(t->pointer++);
            if (byte(t->pointer) < 0x80) gate += byte(t->pointer++);
        }
    }
    if (command == 0xcf) gate = 0;
    pitch_key = (int)t->key;
    if (byte(tone) & 0x40) tone = word(tone + 4) + byte(word(tone + 8) + t->key) * 12;
    else if (byte(tone) & 0x80) {
        tone = word(tone + 4) + t->key * 12;
        pitch_key = byte(tone + 1);
        if (byte(tone + 3) & 0x80) pan = ((int)byte(tone + 3) - 0xc0) * 2;
    }
    if (byte(tone) & 7) { ++skipped; return; }
    for (i=0; i<12; ++i) if (!sound.chans[i].statusFlags) { chosen=i; break; }
    if (chosen == 12) {
        for (i=0; i<12; ++i) if (sound.chans[i].statusFlags & 0x40) { chosen=i; break; }
        if (chosen == 12) chosen = note_count % 12;
    }
    struct SoundChannel *c = sound.chans + chosen;
    DemoVoice *v = voices + chosen;
    uint32_t address = word(tone + 4), size = word(address + 12);
    if (!size || size > 0x400000 || address < 0x08000000 || (uint64_t)(address - 0x08000000) + 16 + size > rom_size || (address & 3)) { failed=1; return; }
    hw_zero(c, sizeof(*c));
    c->wav = (struct WaveData *)(rom + address - 0x08000000); c->type = byte(tone);
    c->attack = byte(tone+8); c->decay = byte(tone+9); c->sustain = byte(tone+10); c->release = byte(tone+11);
    c->statusFlags = 0x80;
    v->track=t; v->gate=gate; v->key=t->key; v->pitch_key=pitch_key; v->pan=pan;
    ++note_count;
}

static void step(DemoTrack *t)
{
    unsigned command = byte(t->pointer);
    if (command < 0x80) command = t->running;
    else { ++t->pointer; if (command >= 0xbd) t->running = command; }
    if (!command) { failed=1; return; }
    if (command >= 0xcf) { note(t,command); return; }
    if (command <= 0xb0) { t->wait = clocks[command-0x80]; return; }
    unsigned a = byte(t->pointer);
    switch (command) {
    case 0xb1: t->on=0; release(t,-1); break;
    case 0xb2: t->pointer=word(t->pointer); break;
    case 0xb3: if (t->depth<3) { t->stack[t->depth++]=t->pointer+4; t->pointer=word(t->pointer); } else t->pointer+=4; break;
    case 0xb4: if(t->depth) t->pointer=t->stack[--t->depth]; break;
    case 0xb5: if (!a || ++t->repeat<a) t->pointer=word(t->pointer+1); else { t->repeat=0; t->pointer+=5; } break;
    case 0xb9: t->pointer+=3; break;
    case 0xba: ++t->pointer; break;
    case 0xbb: tempo=a*2; ++t->pointer; break;
    case 0xbc: t->keyshift=(int8_t)a; ++t->pointer; break;
    case 0xbd: t->program=a; ++t->pointer; break;
    case 0xbe: t->volume=a; ++t->pointer; break;
    case 0xbf: t->pan=(int)a-64; ++t->pointer; break;
    case 0xc0: t->bend=(int)a-64; ++t->pointer; break;
    case 0xc1: t->bend_range=a; ++t->pointer; break;
    case 0xc2: case 0xc3: case 0xc4: case 0xc5: ++t->pointer; break;
    case 0xc8: t->tune=(int)a-64; ++t->pointer; break;
    case 0xcd: t->pointer+=2; break;
    case 0xce: if(a<0x80) { t->key=a; ++t->pointer; } release(t,(int)t->key); break;
    default: break;
    }
}

int demo_start(unsigned song)
{
    unsigned i;
    uint32_t header;
    mode=0; failed=0; note_count=skipped=accumulator=0; tempo=150;
    hw_zero(&sound,sizeof(sound)); hw_zero(tracks,sizeof(tracks)); hw_zero(voices,sizeof(voices));
    if (song>=1483 || rom_size<0x600000) return -1;
    header=word(0x0826456c+song*8); track_count=byte(header);
    if (!track_count || track_count>16 || failed) return -1;
    voicegroup=word(header+4);
    for(i=0;i<track_count;++i) {
        tracks[i].pointer=word(header+8+i*4); tracks[i].on=1;
        tracks[i].volume=100; tracks[i].velocity=127; tracks[i].key=60; tracks[i].bend_range=2;
    }
    sound.maxChans=12; sound.masterVolume=15; sound.pcmFreq=21024; sound.divFreq=399;
    return (int)track_count;
}

void demo_psg(unsigned kind, unsigned frequency)
{
    unsigned i;
    hw_init(&hardware,NULL); mode=1; failed=note_count=skipped=0;
    if(frequency>2047) frequency=2047;
    hw_write16(&hardware,HW_IO_BASE+0x84,0x80);
    hw_write16(&hardware,HW_IO_BASE+0x82,2);
    if(kind==2) {
        hw_write16(&hardware,HW_IO_BASE+0x80,0x4477);
        for(i=0;i<16;++i) hw_write8(&hardware,HW_IO_BASE+0x90+i,(uint8_t)(((i<8?i*2:30-i*2)<<4)|(i<8?i*2+1:31-i*2)));
        hw_write16(&hardware,HW_IO_BASE+0x70,0x80);
        hw_write16(&hardware,HW_IO_BASE+0x72,0x2000);
        hw_write16(&hardware,HW_IO_BASE+0x74,(uint16_t)(0x8000|frequency));
    } else if(kind==3) {
        hw_write16(&hardware,HW_IO_BASE+0x80,0x8877);
        hw_write16(&hardware,HW_IO_BASE+0x78,0xc000);
        hw_write16(&hardware,HW_IO_BASE+0x7c,0x8039);
    } else {
        hw_write16(&hardware,HW_IO_BASE+0x80,0x1177);
        hw_write16(&hardware,HW_IO_BASE+0x62,0xc080);
        hw_write16(&hardware,HW_IO_BASE+0x64,(uint16_t)(0x8000|frequency));
    }
}

unsigned demo_render(void)
{
    unsigned i;
    if(mode) { hw_psg_render(&hardware,pcm,352,21024,0); return 352; }
    accumulator+=tempo;
    while(accumulator>=150 && !failed) {
        accumulator-=150;
        for(i=0;i<12;++i) if(voices[i].gate && --voices[i].gate==0) sound.chans[i].statusFlags|=0x40;
        for(i=0;i<track_count;++i) {
            DemoTrack *t=tracks+i; unsigned guard=0;
            while(t->on && !t->wait && !failed && ++guard<10000) step(t);
            if(guard==10000) failed=1;
            if(t->wait) --t->wait;
        }
    }
    for(i=0;i<12;++i) {
        struct SoundChannel *c=sound.chans+i; DemoVoice *v=voices+i; DemoTrack *t=v->track;
        if(!c->statusFlags || !t) continue;
        int pan=t->pan*2+v->pan; if(pan<-128)pan=-128; if(pan>127)pan=127;
        c->rightVolume=(uint8_t)(((pan+128)*t->volume*2>>8)*t->velocity>>7);
        c->leftVolume=(uint8_t)(((127-pan)*t->volume*2>>8)*t->velocity>>7);
        c->frequency=pitch(c->wav->freq/1024,(v->pitch_key-60+t->keyshift)*256+(t->tune+t->bend*(int)t->bend_range)*4);
    }
    if(failed) { hw_zero(pcm,sizeof(pcm)); return 0; }
    return hw_mp2k_mix(&sound,pcm,352,21024)==HW_OK ? 352 : 0;
}
