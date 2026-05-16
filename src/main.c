
#include <unistd.h>
#include <stdio.h>

#include "dv_glfw.h"
#include "dv_font.h"

const char test_string[] = "If you want your cache to store only one copy "; // per unique string, you must compare the string contents, not the pointers. Thankfully, stb_ds.h does exactly that - if the key is a char *, it automatically uses strcmp() for comparisons and stbds_hash_string() for hashing.";

void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
  (void)mods;
  (void)window;
  (void)scancode;
  if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    dv_glfw_stop_game();
  }
}

char status_buffer[256] = "00000";

int main(void){
  dv_fb_t *screen_data = dv_glfw_init();
  dv_fbr_t screen = dv_fbr_verify(screen_data, 0, 0, screen_data->w, screen_data->h);

  dv_glfw_add_keyfun(&input_callback);

  dv_fb_t *back_image = dv_get_image("bg-frame.png");
  dv_fbr_t screen_clear = dv_fbr_verify(back_image, 0, 0, back_image->w, back_image->h);

  font_t *title_font = font_create("font-title.png", RGBA(0x99,0xDD,0xFF,0xFF), RGBA(0x00,0x00,0x00,0x66));
  font_t *prose_font = font_create("font-prose.png", RGBA(0x99,0xDD,0xFF,0xFF), RGBA(0x00,0x00,0x00,0x66));
  font_t *small_font = font_create("font-small.png", RGBA(0x99,0xDD,0xFF,0xEE), RGBA(0x00,0x00,0x00,0x66));

  int32_t cms = 0, pms = 0, msd = 0, msa = 0, mspf = 10;
  int32_t count = 0;
  while (dv_glfw_keep_going()) {
    glfwPollEvents();

    pms = cms; cms = dv_glfw_get_ticks(); msd = cms - pms; msa += msd;

    if(msa > mspf){
      msa -= mspf; count += 1;
      dv_fbr_blit(screen_clear, screen);
      font_draw_string(title_font, "Welcome to This Game", 24, 24, screen.fb);
      font_draw_string(prose_font, test_string, 24, 48,  screen.fb);
      font_draw_string(small_font, test_string, 32, 272, screen.fb);
    }

    dv_glfw_draw_and_swap();
    sleep(0);
  }

  dv_glfw_exit();
  return 0;
}

