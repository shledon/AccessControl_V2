#include "display.h"
#include "sys.h" 
#include "delay.h"
#include "rc522.h"
#include "nokia_5110.h"
#include "lcdlib.h"
#include "userinfo.h"
#include "retarget.h"
#include "w25qxx.h"
#include "mode.h"

extern u8 NUM_USER;
extern u16 NUM_ROOT;
extern u8 get_root;
extern u8 get_super_root;

/**
 * ��ʾͼƬ1
 */
void showp1()
{
    LCD_Show_Pic(0, 0, 84, 48, pic_heart);
}

/**
 * ��ʾͼƬ2
 */
void showp2()
{
    LCD_Show_Pic(0, 0, 84, 48, pic_dianxie);
}

/**
 * ������ʾ���Ŀ��ţ���ʮ��������ʾ
 * @param p ����
 */
void ShowID(u8 *p)
{
    u8 num[8];
    u8 i;

    for(i=0; i<4; i++)//��4�ֽڵĿ��ţ�32��������λתλ8��16����λ
    {
        num[i*2]=p[i]/16;
        num[i*2]>9?(num[i*2]+='7'):(num[i*2]+='0');
        num[i*2+1]=p[i]%16;
        num[i*2+1]>9?(num[i*2+1]+='7'):(num[i*2+1]+='0');
    }
    printf("ID>>>0x%s\r\n", num);
}

/**
 * ��ʾ�˵�����
 * @param ml Ϊ1ʱ�б������ƶ�һ�У�Ϊ0ʱ�����ƶ�1�У�Ϊ2ʱ���ƶ��б�
 * @param show_char ��ʾ�Ĳ˵�
 * @return ���ص�ǰҳ��һ���������б����������������ڲ˵�ѡ���е�����
 */
u8 show_meanu(u8 ml,char show_char[][15])
{
	u8 i,j;
    static u8 base=0;                      //��ǰҳ��һ���ڲ˵�ѡ���е��б�
    static u8 row=0;                       //��ǰҳ�����б�
	char temp[15];
	if(ml==1)
	{
		if(row == MEANU_MAX-1)         //�б��Ѿ����ﱾҳ���һ��
		{
		    if (base<MEANU_LONG-MEANU_MAX)
		        base++;
		}
		else
			row++;
	}
	else if(ml==0)
	{
		if(row == 0)                   //�б��Ѿ����ﱾҳ��һ��
		{
		    if(base>0)
		        base--;
		}
		else
			row--;
	}

	LCD_clear();				//ÿ�β�������ˢ��һ����ʾ
	for(i=0;i<MEANU_MAX;i++)
	{
		LCD_write_english_string(0,i,show_char[base+i]);
	}

	for(j=0;j<12;j++)
    {
        temp[j]=show_char[row+base][j];
    }
    temp[11]=' ';
	temp[12]=' ';
    temp[13]='<';
	LCD_write_english_string(0,row,temp);
	LCD_write_english_string(0,4,"\\\\\\\\\\\\\\\\\\\\\\\\\\\\");
	LCD_write_english_string(0,5,"    Meanu");

	return base+row;				//���ص�ǰҳ��һ���������б����������������ڲ˵�ѡ���е�����
}

/**
 * ִ�к���
 * @param option ѡ�еĲ˵�ѡ��
 * @param mode ģʽ
 */
void execute(u8 option,u8 *mode)			//ִ�а�����Ӧ����Ϊ
{
    static u8 i;
    //����ģʽ
    if(option==WorkMode)
    {
        i=0;//������ã�ÿ����һ��ҳ����Ƚ�i���㣬֮�����û�а���������i��������i������=5ʱ���Զ�����������
        printf("Work mode\r\n");
        LCD_clear();
        LCD_write_english_string(0,2," Work Mode ");
        LCD_write_english_string(0,0," GDUT  ELC");
        *mode=2;
        initSN();
    }
    //���ģʽ ��Ҫ��ROOTȨ��
    if(option==AddUser)
    {
        i=0;//������ã�ÿ����һ��ҳ����Ƚ�i���㣬֮�����û�а���������i��������i������=5ʱ���Զ�����������
        if(get_root)
        {
            *mode=1;
            printf("ADD mode\r\n");
            LCD_clear();
            LCD_write_english_string(0,2," Add Mode ");
            LCD_write_english_string(0,3," ADD USERS ");
        }
        else
        {
            printf("You are not root!\r\n");
            LCD_clear();
            LCD_write_english_string(0,2," Add Mode ");
            LCD_write_english_string(0,3," Limited!");
        }
        LCD_write_english_string(0,0," GDUT  ELC");
        initSN();
    }
    //ɾ��ģʽ  ��Ҫ��ROOTȨ��
    if(option==DelUser)
    {
        i=0;
        if(get_root)
        {
            printf("Del Mode\r\n");
            LCD_clear();
            LCD_write_english_string(0,2," DEL Mode ");
            *mode=3;
        }
        else
        {
            LCD_clear();
            LCD_write_english_string(0,2," DEL Mode ");
            printf("Not root\r\n");
            LCD_write_english_string(0,3," Limited�� ");
        }
        LCD_write_english_string(0,0," GDUT  ELC");
        initSN();

    }
    //��¼ �� ���ROOT�û�
    if(option==RootLogin)
    {
        i=0;
        LCD_clear();
        LCD_write_english_string(0,0," GDUT  ELC");
        LCD_write_english_string(0,2," Root Login ");
        *mode=4;
        initSN();
    }
    //����Ա�˳�
    if(option==RootOut)
    {
        i=0;
        LCD_clear();
        if(get_root)
        {
            LCD_write_english_string(0,2,"  Logout! ");
        }
        else
        {
            LCD_write_english_string(0,2,"  No Root!");
        }
        get_root=0;     //�˳�����Աģʽ
        get_super_root=0;
    }
    //��ʼ��ϵͳ����������û�
    if(option==InitSystem)
    {
        i=0;
        if(get_root)
        {
            LCD_clear();
            LCD_write_english_string(0,2," All Users ");
            LCD_write_english_string(0,23,"  Clean ");
            W25QXX_Write(0,FLASH_USER_NUM,1);
            NUM_USER=0;					//���û�������
        }
        else
        {
            LCD_clear();
            LCD_write_english_string(0,1," You're not");
            LCD_write_english_string(0,2,"   Root ");
            LCD_write_english_string(0,3,"  Limited ");
        }
        delay_ms(2000);
    }
    //����Ա��ʼ����������й���Ա����Ҫ��SUPER_ROOTȨ��
    if(option==RootInit)
    {
        i=0;
        if(get_super_root)
        {
            LCD_clear();
            LCD_write_english_string(0,2," Clean Root ");
            W25QXX_Write(0,FlASH_ROOT_NUM,1);
            NUM_ROOT=0;					//������Ա������
        }
        else
        {
            LCD_clear();
            LCD_write_english_string(0,1," You're not");
            LCD_write_english_string(0,2,"   Root ");
            LCD_write_english_string(0,3,"  Limited ");
        }
        delay_ms(2000);
    }

    if(option==MakerInfo)
    {
        *mode=MakerInfo;
    }

    if(option==SystemInfo)
    {
        *mode=SystemInfo;
    }

    i++;
    delay_ms(150);

    if(i==5)				//��ʾ��ҳ��
    {
        LED0=!LED0;
        LCD_clear();
        LCD_Show_Pic(0, 0, 84, 48, pic_map);			    //�������ͼͼ��
        if(get_root)
            LCD_Show_Pic(0, 0, 12, 12, pic_unlock);		//���������Ͻǽ���ͼ��
        if(*mode==2)
            LCD_Show_Pic(36, 0, 12, 12, w_mode);		    //�������һ�о���ģʽͼ��
        else if(*mode==4)
            LCD_Show_Pic(36, 0, 12, 12, r_mode);
        else if(*mode==1)
            LCD_Show_Pic(36, 0, 12, 12, a_mode);
        i=0;
    }
}