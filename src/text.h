#ifndef TEXT_H
#define TEXT_H

#define TEXT_STYLE_NORMAL 0
#define TEXT_STYLE_BOLD 1
#define TEXT_STYLE_ITALIC 2
#define TEXT_STYLE_BOLD_AND_ITALIC 3

void initialize_text();
void start_text_rendering();
void end_text_rendering();
void show_text(const char* text, float x, float y, float size=0.04, int style = TEXT_STYLE_NORMAL);
float text_width(const char* text, float size=0.04);

#endif