#include "dv_fb.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

dv_fb_t *dv_fb_create(uint32_t w, uint32_t h){
  dv_fb_t *fb = calloc( 1, sizeof( dv_fb_t ) );
  fb->w = w; fb->h = h;
  fb->pixels = calloc( w * h, sizeof(uint32_t) );
  return fb;
}

dv_fb_t *dv_get_image(const char *fn){
  int w, h, n;
  unsigned char *data = stbi_load(fn, &w, &h, &n, 4);

  if(data == NULL){ return NULL; }

  dv_fb_t *image = dv_fb_create(w,h);
  memcpy(image->pixels,data,w*h*sizeof(uint32_t));

  stbi_image_free(data);

  return image;
}

dv_fbr_t dv_fbr_verify(dv_fb_t *fb, uint32_t x, uint32_t y, uint32_t w, uint32_t h){
  assert( fb != NULL);
  assert( x < fb->w );
  assert( y < fb->h );
  assert( w <= (fb->w - x) );
  assert( h <= (fb->h - y) );

  dv_fbr_t fbr;

  fbr.fb = fb;
  fbr.x = x;
  fbr.y = y;
  fbr.w = w;
  fbr.h = h;

  return fbr;
}

static inline uint32_t blend(uint32_t src, uint32_t dest) {
  uint8_t src_a = (src >> 24) & 0xFF;

  uint8_t src_r = (src >> 16) & 0xFF;
  uint8_t src_g = (src >> 8)  & 0xFF;
  uint8_t src_b =  src        & 0xFF;

  uint8_t dest_r = (dest >> 16) & 0xFF;
  uint8_t dest_g = (dest >> 8)  & 0xFF;
  uint8_t dest_b =  dest        & 0xFF;

  float alpha = src_a / 255.0f;

  uint8_t out_r = (uint8_t)(src_r * alpha + dest_r * (1.0f - alpha));
  uint8_t out_g = (uint8_t)(src_g * alpha + dest_g * (1.0f - alpha));
  uint8_t out_b = (uint8_t)(src_b * alpha + dest_b * (1.0f - alpha));

  return (0xFF << 24) | (out_r << 16) | (out_g << 8) | out_b;
}

void dv_fbr_blit(dv_fbr_t src, dv_fbr_t dest) {
  uint32_t draw_w = (src.w < dest.w) ? src.w : dest.w;
  uint32_t draw_h = (src.h < dest.h) ? src.h : dest.h;

  uint32_t src_parent_w = src.fb->w;
  uint32_t dest_parent_w = dest.fb->w;

  uint32_t (*src_grid)[src_parent_w] = (void*) src.fb->pixels;
  uint32_t (*dest_grid)[dest_parent_w] = (void*) dest.fb->pixels;

  for (uint32_t y = 0; y < draw_h; y++) {
    for (uint32_t x = 0; x < draw_w; x++) {
      uint32_t pixel = src_grid[src.y + y][src.x + x];
      uint32_t alpha = (pixel >> 24) & 0xFF;

      if (alpha == 0) continue;

      if (alpha == 255) {
        dest_grid[dest.y + y][dest.x + x] = pixel;
      } else {
        dest_grid[dest.y + y][dest.x + x] = blend(pixel, dest_grid[dest.y + y][dest.x + x]);
      }
    }
  }
}

