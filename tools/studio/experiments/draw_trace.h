#ifndef STUDIO_EXPERIMENT_DRAW_TRACE_H
#define STUDIO_EXPERIMENT_DRAW_TRACE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef STUDIO_EXPERIMENTS
struct mCore;
bool draw_trace_command(struct mCore *core, const char *directory, const char *line);
bool draw_trace_active(void);
void draw_trace_step(uint32_t pc, uint32_t sp, uint32_t lr, uint32_t arg0, unsigned mode);
void draw_trace_dma(uint32_t address, uint32_t value, uint32_t pc);
void draw_trace_close(void);
#else
#define draw_trace_command(core, directory, line) false
#define draw_trace_active() false
#define draw_trace_step(pc, sp, lr, arg0, mode) ((void)0)
#define draw_trace_dma(address, value, pc) ((void)0)
#define draw_trace_close() ((void)0)
#endif

#endif
