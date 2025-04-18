#include "ui.h"
#include "lvgl.h"

// Define pointers to UI elements
lv_obj_t *ui_lbl_time;
lv_obj_t *ui_lbl_date;
lv_obj_t *ui_btn_m;
lv_obj_t *ui_btn_h;
lv_obj_t *ui_btn_d;
lv_obj_t *ui_img_album_cover;
lv_obj_t *ui_btn_prev;
lv_obj_t *ui_btn_playpause;
lv_obj_t *ui_btn_next;

// Dark theme colors
#define DARK_BG_COLOR       0x181818 // Dark background
#define DARK_ACCENT_COLOR   0x303030 // Slightly lighter for containers
#define TEXT_COLOR          0xFFFFFF // White text
#define TEXT_SECONDARY      0xAAAAAA // Gray text for secondary info
#define ACCENT_GREEN        0x1DB954 // Spotify green
#define ACCENT_RED          0xE53935 // Red for errors

// Define styles
static lv_style_t style_btn_red;
static lv_style_t style_btn_green;
static lv_style_t style_spotify_container;
static lv_style_t style_btn_spotify_green;

static void init_styles() {
    // Red button style
    lv_style_init(&style_btn_red);
    lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_opa(&style_btn_red, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn_red, lv_color_white());

    // Green button style (for checked state)
    lv_style_init(&style_btn_green);
    lv_style_set_bg_color(&style_btn_green, lv_palette_main(LV_PALETTE_GREEN));
    lv_style_set_bg_opa(&style_btn_green, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn_green, lv_color_white());

    // Spotify container style (green theme)
    lv_style_init(&style_spotify_container);
    lv_style_set_bg_color(&style_spotify_container, lv_color_hex(ACCENT_GREEN)); // Spotify green
    lv_style_set_bg_opa(&style_spotify_container, LV_OPA_10); // Light background
    lv_style_set_border_color(&style_spotify_container, lv_color_hex(ACCENT_GREEN)); // Spotify green
    lv_style_set_border_width(&style_spotify_container, 2); // Border thickness
    lv_style_set_border_opa(&style_spotify_container, LV_OPA_50); // Semi-transparent border
    lv_style_set_radius(&style_spotify_container, 8); // Rounded corners
    lv_style_set_pad_all(&style_spotify_container, 5); // Minimal inner padding

    // Spotify button style (green)
    lv_style_init(&style_btn_spotify_green);
    lv_style_set_bg_color(&style_btn_spotify_green, lv_color_hex(ACCENT_GREEN)); // Spotify green
    lv_style_set_bg_opa(&style_btn_spotify_green, LV_OPA_COVER);
    lv_style_set_text_color(&style_btn_spotify_green, lv_color_black()); // Black text/icon for contrast
    lv_style_set_radius(&style_btn_spotify_green, 22); // Fully rounded corners
    lv_style_set_shadow_width(&style_btn_spotify_green, 10); // Keep shadow
    lv_style_set_shadow_opa(&style_btn_spotify_green, LV_OPA_30); // Keep shadow opacity
}

void ui_init(void) {
    init_styles();

    // Set a proper dark theme with more contrast
    lv_theme_t *theme = lv_theme_default_init(lv_disp_get_default(), 
                                             lv_palette_main(LV_PALETTE_BLUE), 
                                             lv_palette_main(LV_PALETTE_RED), 
                                             true,  // Dark theme
                                             LV_FONT_DEFAULT);
    lv_disp_set_theme(lv_disp_get_default(), theme);

    // Force the screen background to pure black for AMOLED
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(DARK_BG_COLOR), 0);
    lv_obj_set_style_bg_opa(screen, LV_OPA_COVER, 0);
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(screen, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(screen, 5, 0);

    // --- Column 1: Clock ---
    lv_obj_t *col1 = lv_obj_create(screen);
    lv_obj_set_size(col1, LV_PCT(28), LV_PCT(95)); // Column width
    lv_obj_set_flex_flow(col1, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(col1, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_style(col1, NULL, LV_PART_SCROLLBAR);
    lv_obj_clear_flag(col1, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(col1, 0, 0);
    lv_obj_set_style_bg_opa(col1, LV_OPA_TRANSP, 0);

    // Create time label with large font
    ui_lbl_time = lv_label_create(col1);
    lv_label_set_text(ui_lbl_time, "00:00");
    lv_obj_set_style_text_font(ui_lbl_time, &lv_font_montserrat_48, 0); // Large font
    lv_obj_set_style_text_color(ui_lbl_time, lv_color_hex(TEXT_COLOR), 0);

    // Create date label below time
    ui_lbl_date = lv_label_create(col1);
    lv_label_set_text(ui_lbl_date, "01.01.70");
    lv_obj_set_style_text_font(ui_lbl_date, &lv_font_montserrat_20, 0); // Smaller font
    lv_obj_set_style_text_color(ui_lbl_date, lv_color_hex(TEXT_SECONDARY), 0);
    lv_obj_set_style_pad_top(ui_lbl_date, 10, 0); // Space between time and date

    // --- Column 2: Discord-style Buttons ---
    lv_obj_t *col2 = lv_obj_create(screen);
    lv_obj_set_size(col2, LV_PCT(28), LV_PCT(95)); // Column width
    lv_obj_set_flex_flow(col2, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(col2, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_style(col2, NULL, LV_PART_SCROLLBAR);
    lv_obj_clear_flag(col2, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(col2, 0, 0);
    lv_obj_set_style_bg_opa(col2, LV_OPA_TRANSP, 0);

    // Button M (Red/Green)
    ui_btn_m = lv_btn_create(col2);
    lv_obj_add_style(ui_btn_m, &style_btn_red, LV_STATE_DEFAULT);
    lv_obj_add_style(ui_btn_m, &style_btn_green, LV_STATE_CHECKED);
    lv_obj_add_flag(ui_btn_m, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_size(ui_btn_m, 60, 60); // Square buttons
    lv_obj_t *lbl_m = lv_label_create(ui_btn_m);
    lv_label_set_text(lbl_m, "m");
    lv_obj_set_style_text_font(lbl_m, &lv_font_montserrat_24, 0); // Larger text
    lv_obj_center(lbl_m);

    // Button H (Red/Green)
    ui_btn_h = lv_btn_create(col2);
    lv_obj_add_style(ui_btn_h, &style_btn_red, LV_STATE_DEFAULT);
    lv_obj_add_style(ui_btn_h, &style_btn_green, LV_STATE_CHECKED);
    lv_obj_add_flag(ui_btn_h, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_set_size(ui_btn_h, 60, 60); // Square buttons
    lv_obj_t *lbl_h = lv_label_create(ui_btn_h);
    lv_label_set_text(lbl_h, "h");
    lv_obj_set_style_text_font(lbl_h, &lv_font_montserrat_24, 0); // Larger text
    lv_obj_center(lbl_h);

    // Button D (Red only)
    ui_btn_d = lv_btn_create(col2);
    lv_obj_add_style(ui_btn_d, &style_btn_red, LV_STATE_DEFAULT);
    lv_obj_set_size(ui_btn_d, 70, 70); // Slightly larger button
    lv_obj_t *lbl_d = lv_label_create(ui_btn_d);
    lv_label_set_text(lbl_d, "d");
    lv_obj_set_style_text_font(lbl_d, &lv_font_montserrat_28, 0); // Larger text
    lv_obj_center(lbl_d);

    // --- Column 3: Audio Control (Spotify) ---
    lv_obj_t *col3 = lv_obj_create(screen);
    lv_obj_set_size(col3, LV_PCT(35), LV_PCT(95)); // Wider column
    lv_obj_set_flex_flow(col3, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(col3, LV_FLEX_ALIGN_SPACE_AROUND, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_remove_style(col3, NULL, LV_PART_SCROLLBAR);
    lv_obj_clear_flag(col3, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(col3, 0, 0);
    lv_obj_set_style_bg_opa(col3, LV_OPA_TRANSP, 0);

    // Create a container for all Spotify elements with green theme
    lv_obj_t *spotify_container = lv_obj_create(col3);
    lv_obj_set_size(spotify_container, LV_PCT(99), LV_PCT(98)); // Fill almost entire column
    lv_obj_set_flex_flow(spotify_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(spotify_container, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_style(spotify_container, &style_spotify_container, 0);
    lv_obj_clear_flag(spotify_container, LV_OBJ_FLAG_SCROLLABLE);

    // Album Cover Image
    ui_img_album_cover = lv_img_create(spotify_container);
    lv_obj_set_size(ui_img_album_cover, 100, 100);
    lv_obj_set_style_bg_color(ui_img_album_cover, lv_color_hex(DARK_ACCENT_COLOR), 0);
    lv_obj_set_style_bg_opa(ui_img_album_cover, LV_OPA_50, 0);
    lv_obj_set_style_border_width(ui_img_album_cover, 1, 0);
    lv_obj_set_style_border_color(ui_img_album_cover, lv_color_hex(0x444444), 0);
    lv_obj_set_style_radius(ui_img_album_cover, 5, 0); // Slight rounding on album art

    // Audio Buttons Container
    lv_obj_t *audio_btn_cont = lv_obj_create(spotify_container);
    lv_obj_remove_style(audio_btn_cont, NULL, LV_PART_SCROLLBAR);
    lv_obj_clear_flag(audio_btn_cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_border_width(audio_btn_cont, 0, 0);
    lv_obj_set_width(audio_btn_cont, LV_PCT(100));
    lv_obj_set_height(audio_btn_cont, LV_SIZE_CONTENT);
    lv_obj_set_flex_flow(audio_btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(audio_btn_cont, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(audio_btn_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_pad_column(audio_btn_cont, 20, 0); // More spacing between buttons
    lv_obj_set_style_pad_top(audio_btn_cont, 15, 0); // Space above buttons

    // Previous Button
    ui_btn_prev = lv_btn_create(audio_btn_cont);
    lv_obj_add_style(ui_btn_prev, &style_btn_spotify_green, 0);
    lv_obj_set_size(ui_btn_prev, 45, 45); // Perfectly round buttons
    lv_obj_t *lbl_prev = lv_label_create(ui_btn_prev);
    lv_label_set_text(lbl_prev, LV_SYMBOL_PREV);
    lv_obj_center(lbl_prev);

    // Play/Pause Button
    ui_btn_playpause = lv_btn_create(audio_btn_cont);
    lv_obj_add_style(ui_btn_playpause, &style_btn_spotify_green, 0);
    lv_obj_set_size(ui_btn_playpause, 45, 45);
    lv_obj_add_flag(ui_btn_playpause, LV_OBJ_FLAG_CHECKABLE);
    lv_obj_t *lbl_playpause = lv_label_create(ui_btn_playpause);
    lv_label_set_text(lbl_playpause, LV_SYMBOL_PAUSE);
    lv_obj_center(lbl_playpause);

    // Next Button
    ui_btn_next = lv_btn_create(audio_btn_cont);
    lv_obj_add_style(ui_btn_next, &style_btn_spotify_green, 0);
    lv_obj_set_size(ui_btn_next, 45, 45);
    lv_obj_t *lbl_next = lv_label_create(ui_btn_next);
    lv_label_set_text(lbl_next, LV_SYMBOL_NEXT);
    lv_obj_center(lbl_next);
}

