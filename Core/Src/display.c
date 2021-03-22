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
 * 显示图片1
 */
void showp1()
{
    LCD_Show_Pic(0, 0, 84, 48, pic_heart);
}

/**
 * 显示图片2
 */
void showp2()
{
    LCD_Show_Pic(0, 0, 84, 48, pic_dianxie);
}

/**
 * 串口显示卡的卡号，以十六进制显示
 * @param p 卡号
 */
void ShowID(u8 *p)
{
    u8 num[8];
    u8 i;

    for(i=0; i<4; i++)//将4字节的卡号，32个二进制位转位8个16进制位
    {
        num[i*2]=p[i]/16;
        num[i*2]>9?(num[i*2]+='7'):(num[i*2]+='0');
        num[i*2+1]=p[i]%16;
        num[i*2+1]>9?(num[i*2+1]+='7'):(num[i*2+1]+='0');
    }
    printf("ID>>>0x%s\r\n", num);
}

/**
 * 显示菜单函数
 * @param ml 为1时行标向下移动一行，为0时向上移动1行，为2时不移动行标
 * @param show_char 显示的菜单
 * @return 返回当前页第一行行数和行标所在行行数，即在菜单选项中的排序
 */
u8 show_meanu(u8 ml,char show_char[][15])
{
	u8 i,j;
    static u8 base=0;                      //当前页第一行在菜单选项中的行标
    static u8 row=0;                       //当前页所在行标
	char temp[15];
	if(ml==1)
	{
		if(row == MEANU_MAX-1)         //行标已经到达本页最后一行
		{
		    if (base<MEANU_LONG-MEANU_MAX)
		        base++;
		}
		else
			row++;
	}
	else if(ml==0)
	{
		if(row == 0)                   //行标已经到达本页第一行
		{
		    if(base>0)
		        base--;
		}
		else
			row--;
	}

	LCD_clear();				//每次操作都会刷新一次显示
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

	return base+row;				//返回当前页第一行行数和行标所在行行数，即在菜单选项中的排序
}

/**
 * 执行函数
 * @param option 选中的菜单选项
 * @param mode 模式
 */
void execute(u8 option,u8 *mode)			//执行按键对应的行为
{
    static u8 i;
    //工作模式
    if(option==WorkMode)
    {
        i=0;//做标记用，每进入一个页面后先将i归零，之后如果没有按键按下则i递增，当i递增到=5时，自动返回主界面
        printf("Work mode\r\n");
        LCD_clear();
        LCD_write_english_string(0,2," Work Mode ");
        LCD_write_english_string(0,0," GDUT  ELC");
        *mode=2;
        initSN();
    }
    //添加模式 需要有ROOT权限
    if(option==AddUser)
    {
        i=0;//做标记用，每进入一个页面后先将i归零，之后如果没有按键按下则i递增，当i递增到=5时，自动返回主界面
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
    //删除模式  需要有ROOT权限
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
            LCD_write_english_string(0,3," Limited！ ");
        }
        LCD_write_english_string(0,0," GDUT  ELC");
        initSN();

    }
    //登录 或 添加ROOT用户
    if(option==RootLogin)
    {
        i=0;
        LCD_clear();
        LCD_write_english_string(0,0," GDUT  ELC");
        LCD_write_english_string(0,2," Root Login ");
        *mode=4;
        initSN();
    }
    //管理员退出
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
        get_root=0;     //退出管理员模式
        get_super_root=0;
    }
    //初始化系统，清除所有用户
    if(option==InitSystem)
    {
        i=0;
        if(get_root)
        {
            LCD_clear();
            LCD_write_english_string(0,2," All Users ");
            LCD_write_english_string(0,23,"  Clean ");
            W25QXX_Write(0,FLASH_USER_NUM,1);
            NUM_USER=0;					//将用户数清零
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
    //管理员初始化，清除所有管理员，需要有SUPER_ROOT权限
    if(option==RootInit)
    {
        i=0;
        if(get_super_root)
        {
            LCD_clear();
            LCD_write_english_string(0,2," Clean Root ");
            W25QXX_Write(0,FlASH_ROOT_NUM,1);
            NUM_ROOT=0;					//将管理员数清零
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

    if(i==5)				//显示主页面
    {
        LED0=!LED0;
        LCD_clear();
        LCD_Show_Pic(0, 0, 84, 48, pic_map);			    //主界面地图图像
        if(get_root)
            LCD_Show_Pic(0, 0, 12, 12, pic_unlock);		//主界面左上角解锁图标
        if(*mode==2)
            LCD_Show_Pic(36, 0, 12, 12, w_mode);		    //主界面第一行居中模式图标
        else if(*mode==4)
            LCD_Show_Pic(36, 0, 12, 12, r_mode);
        else if(*mode==1)
            LCD_Show_Pic(36, 0, 12, 12, a_mode);
        i=0;
    }
}