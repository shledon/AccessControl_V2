#include "nokia_5110.h"
#include "delay.h"
#include "bmp_pixel.h"
#include "write_chinese_string_pixel.h"
#include "english_6x8_pixel.h"
#include "main.h"

void LCD_IO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitStruct.Pin = LCD_CE_Pin|LCD_DC_Pin|LCD_DIN_Pin|LCD_CLK_Pin
                          |CONTROL_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = LCD_RST_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(LCD_RST_GPIO_Port, &GPIO_InitStruct);

    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOB, LCD_CE_Pin|LCD_DC_Pin|LCD_DIN_Pin|LCD_CLK_Pin, GPIO_PIN_SET);
}


void LCD_init(void)
{
    LCD_IO_Init();
    // 产生一个让LCD复位的低电平脉冲
    LCD_RST = 0;
    delay_us(1);
    LCD_RST = 1;
    
    // 关闭LCD
    LCD_CE = 0;
    delay_us(1);
    // 使能LCD
    LCD_CE = 1;
    delay_us(1);

    LCD_write_byte(0x21, 0);	// 使用扩展命令设置LCD模式
    LCD_write_byte(0xc8, 0);	// 设置偏置电压
    LCD_write_byte(0x06, 0);	// 温度校正
    LCD_write_byte(0x13, 0);	// 1:48
    LCD_write_byte(0x20, 0);	// 使用基本命令
    LCD_clear();	                    // 清屏
    LCD_write_byte(0x0c, 0);	// 设定显示模式，正常显示
        
    // 关闭LCD
    LCD_CE = 0;
  }

/*-----------------------------------------------------------------------
LCD_clear         : LCD清屏函数

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/
void LCD_clear(void)
  {
    unsigned int i;

    LCD_write_byte(0x0c, 0);			
    LCD_write_byte(0x80, 0);			

    for (i=0; i<504; i++)
      LCD_write_byte(0, 1);			
  }

/*-----------------------------------------------------------------------
LCD_set_XY        : 设置LCD坐标函数

输入参数：X       ：0－83
          Y       ：0－5

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/
void LCD_set_XY(unsigned char X, unsigned char Y)
  {
    LCD_write_byte(0x40 | Y, 0);		// column
    LCD_write_byte(0x80 | X, 0);          	// row
  }

/*-----------------------------------------------------------------------
LCD_write_char    : 显示英文字符

输入参数：c       ：显示的字符；

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 
-----------------------------------------------------------------------*/
void LCD_write_char(unsigned char c)
  {
    unsigned char line;

    c -= 32;

    for (line=0; line<6; line++)
      LCD_write_byte(font6x8[c][line], 1);
  }

/*-----------------------------------------------------------------------
LCD_write_english_String  : 英文字符串显示函数

输入参数：*s      ：英文字符串指针；
          X、Y    : 显示字符串的位置,x 0-83 ,y 0-5

编写日期          ：2004-8-10 
最后修改日期      ：2004-8-10 		
-----------------------------------------------------------------------*/
void LCD_write_english_string(unsigned char X,unsigned char Y,char *s)
  {
    LCD_set_XY(X,Y);
    while (*s) 
      {
	 LCD_write_char(*s);
	 s++;
      }
  }

/*-----------------------------------------------------------------------
LCD_draw_map      : 位图绘制函数

输入参数：X、Y    ：位图绘制的起始X、Y坐标；
          *map    ：位图点阵数据；
          Pix_x   ：位图像素（长）
          Pix_y   ：位图像素（宽）

编写日期          ：2004-8-13
最后修改日期      ：2004-8-13 
-----------------------------------------------------------------------*/
/**
 *
 * @param x 绘制的起始X、Y坐标；
 * @param y 绘制的起始X、Y坐标；
 * @param width
 * @param height
 * @param pic 位图点阵数据
 */
void LCD_Show_Pic(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t *pic)
{
    uint8_t i, j, row;
    if(height>48)
        height=48;
    if(height%8)
        row = height/8+1;
    else
        row = height/8;
    for(j=0; j<row; j++)
    {
        LCD_set_XY(x,y+j);
        for(i=0; i<width; i++)
        {
            LCD_write_byte(*pic,1);
            pic++;
        }
    }
}

/**
 * 使用SPI接口写数据到LCD
 * @param dat 写入的数据；
 * @param command 写数据/命令选择；写数据为1，写命令为0；
 */
void LCD_write_byte(unsigned char dat, unsigned char command)
{
    unsigned char i;
    // 使能LCD
    LCD_CE = 0;
    
    if (command == 0)
    // 传送命令
     LCD_DC = 0;
    else
    // 传送数据
     LCD_DC = 1;
		for(i=0;i<8;i++)
		{
			if(dat&0x80)
				SDIN = 1;
			else
				SDIN = 0;
			SCLK = 0;
			dat = dat << 1;
			SCLK = 1;
		}
     LCD_CE = 1;
}

