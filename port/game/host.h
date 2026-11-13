#ifndef BOKTAI_PORT_HOST_H
#define BOKTAI_PORT_HOST_H
#include <stdint.h>
/* Functions the page (or Node test host) provides to the full-game module. */
#if defined(__wasm__)
#define PORT_IMPORT(name) __attribute__((import_module("env"), import_name(#name)))
#else
#define PORT_IMPORT(name)
#endif
/* Present a finished 240x160 RGB555 frame and pace to 60 Hz. Returns the
 * pressed-key mask in KEYINPUT bit order (1 = pressed, A = bit 0). */
PORT_IMPORT(frame) uint32_t port_host_frame(const uint16_t *pixels, uint32_t frame);
/* Interleaved signed 16-bit stereo PCM produced during the last frame. */
PORT_IMPORT(audio) void port_host_audio(const int16_t *stereo, uint32_t frames, uint32_t rate);
PORT_IMPORT(log) void port_host_log(const char *message, uint32_t value);
/* Unrecoverable: the module traps right after this returns. */
PORT_IMPORT(fatal) void port_host_fatal(const char *message, uint32_t value);
#endif
