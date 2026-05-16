#pragma once

#include <stdint.h>

#define RGBA(r,g,b,a) ((uint32_t)( ((a) << 24) | ((b) << 16) | ((g) << 8) | (r) ))

typedef struct{
  uint32_t w, h;
  uint32_t *pixels;
} dv_fb_t;

dv_fb_t *dv_fb_create(uint32_t w, uint32_t h);
dv_fb_t *dv_get_image(const char *fn);

typedef struct{
  dv_fb_t *fb;
  uint32_t x, y, w, h;
} dv_fbr_t;

dv_fbr_t dv_fbr_verify(dv_fb_t *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h);

void dv_fbr_blit(dv_fbr_t src, dv_fbr_t dest);
