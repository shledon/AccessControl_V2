#include "sys.h"

#define SCLK PBout(11)
#define SDIN PBout(10)
#define LCD_DC PBout(1)
#define LCD_CE PBout(0)
#define LCD_RST PCout(5)

void LCD_Show_Pic(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t *pic);
void LCD_IO_Init(void);
void LCD_init(void);
void LCD_clear(void);
void LCD_move_chinese_string(unsigned char X, unsigned char Y, unsigned char T); 
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s);
void LCD_write_chinese_string(unsigned char X, unsigned char Y,
                   unsigned char ch_with,unsigned char num,
                   unsigned char line,unsigned char row);
void chinese_string(unsigned char X, unsigned char Y, unsigned char T);                 
void LCD_write_char(unsigned char c);
void LCD_draw_bmp_pixel(unsigned char X,unsigned char Y,unsigned char *map,
                  unsigned char Pix_x,unsigned char Pix_y);
void LCD_write_byte(unsigned char dat, unsigned char dc);
void LCD_set_XY(unsigned char X, unsigned char Y);


