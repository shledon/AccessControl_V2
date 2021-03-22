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
    // ����һ����LCD��λ�ĵ͵�ƽ����
    LCD_RST = 0;
    delay_us(1);
    LCD_RST = 1;
    
    // �ر�LCD
    LCD_CE = 0;
    delay_us(1);
    // ʹ��LCD
    LCD_CE = 1;
    delay_us(1);

    LCD_write_byte(0x21, 0);	// ʹ����չ��������LCDģʽ
    LCD_write_byte(0xc8, 0);	// ����ƫ�õ�ѹ
    LCD_write_byte(0x06, 0);	// �¶�У��
    LCD_write_byte(0x13, 0);	// 1:48
    LCD_write_byte(0x20, 0);	// ʹ�û�������
    LCD_clear();	                    // ����
    LCD_write_byte(0x0c, 0);	// �趨��ʾģʽ��������ʾ
        
    // �ر�LCD
    LCD_CE = 0;
  }

/*-----------------------------------------------------------------------
LCD_clear         : LCD��������

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
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
LCD_set_XY        : ����LCD���꺯��

���������X       ��0��83
          Y       ��0��5

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
-----------------------------------------------------------------------*/
void LCD_set_XY(unsigned char X, unsigned char Y)
  {
    LCD_write_byte(0x40 | Y, 0);		// column
    LCD_write_byte(0x80 | X, 0);          	// row
  }

/*-----------------------------------------------------------------------
LCD_write_char    : ��ʾӢ���ַ�

���������c       ����ʾ���ַ���

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 
-----------------------------------------------------------------------*/
void LCD_write_char(unsigned char c)
  {
    unsigned char line;

    c -= 32;

    for (line=0; line<6; line++)
      LCD_write_byte(font6x8[c][line], 1);
  }

/*-----------------------------------------------------------------------
LCD_write_english_String  : Ӣ���ַ�����ʾ����

���������*s      ��Ӣ���ַ���ָ�룻
          X��Y    : ��ʾ�ַ�����λ��,x 0-83 ,y 0-5

��д����          ��2004-8-10 
����޸�����      ��2004-8-10 		
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
LCD_draw_map      : λͼ���ƺ���

���������X��Y    ��λͼ���Ƶ���ʼX��Y���ꣻ
          *map    ��λͼ�������ݣ�
          Pix_x   ��λͼ���أ�����
          Pix_y   ��λͼ���أ���

��д����          ��2004-8-13
����޸�����      ��2004-8-13 
-----------------------------------------------------------------------*/
/**
 *
 * @param x ���Ƶ���ʼX��Y���ꣻ
 * @param y ���Ƶ���ʼX��Y���ꣻ
 * @param width
 * @param height
 * @param pic λͼ��������
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
 * ʹ��SPI�ӿ�д���ݵ�LCD
 * @param dat д������ݣ�
 * @param command д����/����ѡ��д����Ϊ1��д����Ϊ0��
 */
void LCD_write_byte(unsigned char dat, unsigned char command)
{
    unsigned char i;
    // ʹ��LCD
    LCD_CE = 0;
    
    if (command == 0)
    // ��������
     LCD_DC = 0;
    else
    // ��������
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

