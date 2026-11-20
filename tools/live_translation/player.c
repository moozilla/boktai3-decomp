/* Minimal local player. SDL presents mGBA's unmodified video and stereo samples. */
#include <SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static SDL_Window *window;
static SDL_Renderer *renderer;
static SDL_Texture *texture;
static SDL_AudioDeviceID audio;
static unsigned hz;
static int paused;
static uint32_t keys;

int lt_window(void)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) return 0;
    SDL_InitSubSystem(SDL_INIT_AUDIO);
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
    window = SDL_CreateWindow("Boktai 3  |  Live English", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, 960, 640,
                              SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) return 0;
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if (!renderer) renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_SOFTWARE);
    if (!renderer) return 0;
    SDL_RenderSetLogicalSize(renderer,240,160);
    texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_ABGR8888,
                                SDL_TEXTUREACCESS_STREAMING,240,160);
    paused = 0; keys = 0;
    return texture != NULL;
}
const char *lt_window_error(void) { return SDL_GetError(); }
void lt_title(const char *s) { SDL_SetWindowTitle(window,s); }
void lt_present(const uint32_t *pixels)
{
    SDL_UpdateTexture(texture,NULL,pixels,240*4);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer,texture,NULL,NULL);
    SDL_RenderPresent(renderer);
}
void lt_audio(const int16_t *data, unsigned frames, unsigned rate)
{
    if (!rate) return;
    if (rate != hz) {
        if (audio) SDL_CloseAudioDevice(audio);
        SDL_AudioSpec spec = {0};
        spec.freq = (int)rate; spec.channels = 2; spec.samples = 512;
        spec.format = AUDIO_S16SYS;
        audio = SDL_OpenAudioDevice(NULL,0,&spec,NULL,0);
        if (audio) SDL_PauseAudioDevice(audio,0);
        hz = rate;
    }
    if (audio && SDL_GetQueuedAudioSize(audio) < rate) SDL_QueueAudio(audio,data,frames*4);
}
static unsigned keymask(SDL_Keycode key)
{
    switch (key) {
    case SDLK_x: return 1;
    case SDLK_z: return 2;
    case SDLK_BACKSPACE: return 4;
    case SDLK_RETURN: return 8;
    case SDLK_RIGHT: return 16;
    case SDLK_LEFT: return 32;
    case SDLK_UP: return 64;
    case SDLK_DOWN: return 128;
    case SDLK_s: return 256;
    case SDLK_a: return 512;
    default: return 0;
    }
}
int lt_input(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: return -1;
        case SDL_WINDOWEVENT:
            if (event.window.event==SDL_WINDOWEVENT_FOCUS_LOST) keys=0;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym==SDLK_ESCAPE) return -1;
            if (!event.key.repeat && event.key.keysym.sym==SDLK_SPACE) paused=!paused;
            keys |= keymask(event.key.keysym.sym);
            break;
        case SDL_KEYUP: keys &= ~keymask(event.key.keysym.sym); break;
        }
    }
    return (int)keys | (paused ? 1024 : 0);
}
void lt_quit_window(void)
{
    if (audio) SDL_CloseAudioDevice(audio);
    SDL_DestroyTexture(texture); SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window);
    SDL_Quit();
    audio=0; hz=0;
}
