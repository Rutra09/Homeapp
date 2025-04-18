#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"

// Declare pointers to UI elements that might need updating
extern lv_obj_t *ui_lbl_time;
extern lv_obj_t *ui_lbl_date;
extern lv_obj_t *ui_btn_m;
extern lv_obj_t *ui_btn_h;
extern lv_obj_t *ui_btn_d;
extern lv_obj_t *ui_img_album_cover;
extern lv_obj_t *ui_btn_prev;
extern lv_obj_t *ui_btn_playpause;
extern lv_obj_t *ui_btn_next;


// Function to initialize the UI
void ui_init(void);

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_H*/
