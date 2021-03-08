#include "mode.h"
#include "sys.h"
#include "rc522.h"
#include "userinfo.h"
#include "remote.h"
#include "display.h"
#include "retarget.h"
#include "nokia_5110.h"
#include "delay.h"
#include "gpio.h"
#include "w25qxx.h"

extern struct USER student[USER_MAX_SIZE];
extern unsigned char SN[4]; //读取到的卡号
extern u8 root[NUM_ROOT_MAX][4];//管理员信息数组

u8 get_root=0;
u8 get_super_root=0;
extern u8 NUM_ROOT;
extern u16 NUM_USER;

/**
 * 将卡号清零
 */
void initSN()
{
    SN[0]=0;
    SN[1]=0;
    SN[2]=0;
    SN[3]=0;
}

/**
 * 将新的管理员卡号写进FLASH和ROOT数组
 */
void write_root_num()
{
    u8 j;

    for(j=0;j<USER_ID_SIZE;j++)
    {
        root[NUM_ROOT][j]=SN[j];
        W25QXX_Write(&SN[j],FlASH_ROOT_DATA+NUM_ROOT*4+j,1);
    }
    //update NUN
    NUM_ROOT++;
    W25QXX_Write(&NUM_ROOT,FlASH_ROOT_NUM,1);
}

/**
 * 检查读取的卡是否与上一次读取的一样
 * @param data 卡号
 * @return
 */
u8 check_read_same()
{
    static u8 temp[4]={0};//只赋一次初值
    int i,j;
    for(i=0;i<4;i++)
    {
        if(temp[i]==SN[i])
            continue;
        for(j=0;j<4;j++)
        {
            temp[j]=SN[j];
        }
        return 0;
    }
    return 1;
}

/**
 * 检查用户是否已经存在
 * @param data 卡号
 */
u8 check_user_exist()
{
    u16 i ;
    for(i=0;i<NUM_USER;i++)
    {
        if(student[i].id[0] == SN[0] && student[i].id[1]==SN[1] && student[i].id[2]==SN[2] && student[i].id[3]==SN[3])
        {
            printf("USER EXIST!\r\n");
            LCD_clear();
            LCD_write_english_string(0,1,"EXIST!");
            return 1;
        }
    }
    return 0 ;
}

/**
 * 检查管理员是否已经存在
 * @param data 卡号
 */
u8 check_root_exist()
{
    u8 i;
    for(i=0;i<NUM_ROOT;i++)
    {
        ShowID(root[i]);
        if(root[i][0] == SN[0] && root[i][1]==SN[1] && root[i][2]==SN[2] && root[i][3]==SN[3])
        {
            printf("Exist\r\n");
            LCD_clear();
            LCD_write_english_string(0,1,"EXIST!");
            delay_ms(1000);
            return 1;
        }
    }
    return 0 ;
}


/**
 * 添加用户，将用户相关信息填写进STUDENT结构体
 * @param idnum 用户ID
 */
u8 add_user(char idnum[])
{
    u16 i,j,flag=0;

    for(i=0;i<NUM_USER;i++)
    {
        flag=0;
        for(j=0;j<USER_INFO_SIZE;j++)                                  //检查输入的ID是否已经存在
        {
            if(student[i].info[j]==(idnum[j*2]-48)+(idnum[j*2+1]-48)*10)//将输入的char类型转化成数字
            {
                flag++;
            }
        }
    }
    if(flag < 5)        //输入的ID不重复，则将ID写进INFO中
    {
        for(i=0;i<USER_INFO_SIZE;i++)
        {
            student[NUM_USER].info[i]=(idnum[i*2]-48)+(idnum[i*2+1]-48)*10;//将2位数压缩为1个字节
        }
        for(i=0;i<USER_ID_SIZE;i++)
        {
            student[NUM_USER].id[i]=SN[i];
        }
        printf("\r\n%d %d %d %d\r\n",SN[0],SN[1],SN[2],SN[3]);
        printf("id=%d %d %d %d\r\n",student[NUM_USER].id[0],student[NUM_USER].id[1],student[NUM_USER].id[2],student[NUM_USER].id[3]);
        NUM_USER++;     //当前用户数加一
        for(i=0;i<11;i++)//添加完毕将数组清零
        {
            idnum[i]=' ';
        }
    }
    else                //输入的ID重复
    {
        printf("Same ID\r\n");
        LCD_write_english_string(0,5,"ID exist");
        delay_ms(1000);
        LCD_write_english_string(0,5,"         ");
        return 0;
    }
    return 1;
}

/**
 * 添加管理员，将管理员相关信息填写进FLASH和ROOT数组
 */
u8 add_root()
{
    RC522_Handel();
    if(!check_read_same())            //检查这次读取的卡是否与管理员登录读取的卡一样，不一样则往下执行
    {
        //满足卡未存在、管理员还未达到最大数则进行登记
        if(!check_root_exist() && NUM_ROOT<=NUM_ROOT_MAX)
        {
            write_root_num();     //将新的管理员卡号写进FLASH和ROOT数组
            LCD_clear();
            printf("Write a new root card\r\n");
            LCD_write_english_string(0,1,"NEW ROOT!");
            delay_ms(1000);
        }
        else if(NUM_ROOT>NUM_ROOT_MAX)
        {
            LCD_clear();
            LCD_write_english_string(0,1,"ROOT MAX!");
            delay_ms(1000);
        }
    }
    else
    {
        LCD_clear();
        printf("Please put a new card!\r\n");
        LCD_write_english_string(0,1,"Please put a new card!");
        delay_ms(1000);
    }
    return 1;
}

/**
 * 删除用户，将用户相关信息从STUDENT结构体中删除
 * @param cur 要删除的ID在所有ID中的排序
 */
u8 del_user(u8 cur)
{
    u8 j;
    for(j=0;j<USER_ID_SIZE;j++)
    {
        student[cur].id[j]=0;
    }
    for(j=0;j<USER_INFO_SIZE;j++)
    {
        student[cur].info[j]=0;
    }
    NUM_USER--;
    return 1;
}

/**
 * 显示所有录入的ID
 * @param ml 为1时向上移动1行，为0时向下移动一行
 * @return 返回当前行标在所有ID中的排序
 */
u8 show_all_id(u8 ml)
{
    u8 i,j;
    static u8 base=0;                           //当前页第一行在所有ID中的行标
    static u8 cur=0;                            //在当前页中的行标
    static char user_id[12]={"           "};


    if(ml==1)
    {
        if(cur==PAGE_MAX-1)                  //行标到达当前页的最后一行
        {
            if(base<NUM_USER-PAGE_MAX)
            {
                base++;
            }
        }
        else
            cur++;
    }
    else if(ml == 0)
    {
        if(cur==0)                           //行标到达当前页的第一行
        {
            if(base >0 )                        //还没到达所有ID的第一行
            {
                base--;                         //将当前页的第一行向上移
            }
        }
        else
            cur--;
    }

    //显示
    for(i=0;i<NUM_USER && i<PAGE_MAX-1;i++)
    {
        for(j=0;j<USER_INFO_SIZE;j++)
        {
            //将info中的6个字节ID解压为12个字节
            user_id[j*2]=student[base+i].info[j]%10+48;      //取十位
            user_id[j*2+1]=student[base+i].info[j]/10+48;    //取个位
        }
        user_id[10]=' ';
        user_id[11]=' ';
        LCD_write_english_string(0,i,user_id);
    }
    //显示标志'<'
    for(j=0;j<USER_INFO_SIZE;j++)
    {
        user_id[j*2]=student[base+cur].info[j]%10+48;
        user_id[j*2+1]=student[base+cur].info[j]/10+48;
    }
    user_id[10]=' ';
    user_id[11]='<';
    if(NUM_USER!=0) LCD_write_english_string(0,cur,user_id);

    return base+cur;             //返回当前行标在所有ID中的排序
}

/**
 * 显示输入的ID
 * @param ml 为1时写进下一个数字，为0时删除上一个数字
 * @param num 红外遥控按下的数字按键
 * @param idnum[] 用户ID
 */
u8 show_id(u8 ml,char num,char idnum[])
{
    static u8 cur=0;                            //列标
    if(ml==1)
    {
        if(cur==USER_ID_LONG-1)         //到达最大输入个数
        {
            cur=USER_ID_LONG-1;
        }
        else
        {
            idnum[cur]=num;
            cur++;
            LCD_write_english_string(0,3,idnum);
        }
    }
    else
    {
        if(cur==0)
        {
            cur=1;
        }
        idnum[cur-1]=' ';
        cur--;
        LCD_write_english_string(0,3,idnum);
    }
    return 1;
}

/**
 * 读卡模式
 */
void read_mode()
{
    u16 i;
    RC522_Handel();   //读卡操作，返回卡号SN[]
    for(i=0;i<NUM_USER;i++)//从所有学生信息中检索是否有匹配
    {
        if(student[i].id[0]==SN[0]&&student[i].id[1]==SN[1]&&student[i].id[2]==SN[2]&&student[i].id[3]==SN[3])
        {
            printf("Open The Door\r\n");
            LCD_clear();
            Door1=1;
            LED1=1;
            showp2();
            delay_ms(1000);
            showp1();
            delay_ms(100);
            Door1=0;
            LED1=0;
            initSN();
        }
    }
}

/**
 * 管理员模式
 */
void root_mode()
{
    u8 i;
    u8 num_flag=1;
    u8 key_val=0;
    u8 super_root[4]={0x5A,0x4B,0x2F,0x15};

    RC522_Handel();
    if(!get_root)           //还没得到管理员权限，进行管理员登录
    {
        if(super_root[0]==SN[0]&&super_root[1]==SN[1]&&super_root[2]==SN[2]&&super_root[3]==SN[3])
        {
            get_super_root=1;
            LCD_clear();
            printf("Successful login\r\n");
            LCD_write_english_string(0,1,"Get Super ROOT!");
            initSN();
        }
        for(i=0;i<NUM_ROOT;i++)
        {
            if(root[i][0]==SN[0]&&root[i][1]==SN[1]&&root[i][2]==SN[2]&&root[i][3]==SN[3])
            {
                get_root=1;
                LCD_clear();
                printf("Successful login\r\n");
                LCD_write_english_string(0,1,"Get ROOT!");
                initSN();
            }
        }
    }
    else        //已得到管理员权限，进行管理员增加
    {
        while(num_flag)
        {
            key_val=Remote_Scan();
            if(!key_val)        //任意按键按下退出
            {
                num_flag=0;
            }
            LCD_clear();
            LCD_write_english_string(0,0,"ADD ROOT");
            add_root();
            LCD_clear();
            LCD_write_english_string(0,2,"Any  ");
            LCD_write_english_string(0,3,"Key");
            LCD_write_english_string(0,4,"Exit");
        }
    }
}

/**
 * 新增模式
 */
void add_mode()
{
    u8 num_flag=1;
    u8 key_val=0;
    char idnum[11]={"          "};       //用来缓存用户ID

    if(get_root)
    {
        RC522_Handel();//读卡
        if( !check_read_same() )//查看是否为同一张卡
        {
            if(!check_user_exist() && NUM_USER<=NUM_USER_MAX)
            {
                LCD_clear();
                LCD_write_english_string(0,0,"New Card!");//找到
                LCD_write_english_string(0,1,"Enter ID:");//找到
                while(num_flag)
                {
                    key_val=Remote_Scan();
                    switch(key_val)
                    {
                        case 104:show_id(1,48,idnum);break;//0
                        case 48:show_id(1,49,idnum);break;//1
                        case 24:show_id(1,50,idnum);break;//2
                        case 122:show_id(1,51,idnum);break;//3
                        case 16:show_id(1,52,idnum);break;//4
                        case 56:show_id(1,53,idnum);break;//5
                        case 90:show_id(1,54,idnum);break;//6
                        case 66:show_id(1,55,idnum);break;//7
                        case 74:show_id(1,56,idnum);break;//8
                        case 82:show_id(1,57,idnum);break;//9
                        case 144:show_id(0,48,idnum);break;//EQ删除上一个数字
                        case 98:num_flag=0;break;//ch   退出添加
                        case 194://>||  确定添加
                            if(add_user(idnum))
                            {
                                num_flag=0;
                                reflash_information();//更新FLASH内容
                            }
                            break;
                        default:num_flag=1;
                    }
                }
                LCD_clear();
                switch(key_val)
                {
                    case 194:
                        printf("Write a new card");
                        LCD_write_english_string(0,2,"New User!");
                        delay_ms(1000);
                        break;
                    case 98:
                        LCD_write_english_string(0,2,"   Quick!");
                        delay_ms(1000);
                        break;
                    default:
                        LCD_write_english_string(0,2,"   Unusual!");//异常情况
                        delay_ms(1000);
                }
            }
        }
    }
}

/**
 * 删除模式
 */
void del_mode()
{
    u8 key_val;             //保存红外键值
    u8 num_flag=1;
    u8 all_id_cur=0;        //在所有已经录入的ID中的排序

    if(get_root)
    {
        if(NUM_USER!=0)            //现存用户数不为0
        {
            LCD_clear();
            show_all_id(2);
            while(num_flag)
            {
                key_val=Remote_Scan();
                switch(key_val)
                {
                    case 168:all_id_cur=show_all_id(1);break;//+ 向上移动一行
                    case 224:all_id_cur=show_all_id(0);break;//- 向下移动一行
                    case 98:num_flag=0;break;//ch 退出删除
                    case 194://>||   确定删除
                        if(del_user(all_id_cur))
                        {
                            num_flag=0;
                            reflash_information();//更新FLASH内容
                        }
                        break;
                    default:num_flag=1;
                }
            }
            LCD_clear();
            switch(key_val)
            {
                case 194:
                    LCD_write_english_string(0,2,"   Del!");
                    delay_ms(1000);
                    break;
                case 98:
                    LCD_write_english_string(0,2,"   Quick!");
                    delay_ms(1000);
                    break;
                default:
                    LCD_write_english_string(0,2,"   Unusual!");
                    delay_ms(1000);
            }
        }
        else
        {
            LCD_write_english_string(0,2," NULL!");
            delay_ms(1000);
        }
    }
}

/**
 * 菜单页模式
 * @param pmode 选中的菜单中的选项
 */
void meanu_mode(u8* pmode)
{
    char meanu[MEANU_LONG][15]={"Work Mode  ",
                                "Add  User  ",
                                "Del  User  ",
                                "Root Login ",
                                "Root  Out  ",
                                "Root init   ",
                                "Init System  ",
                                "System Info  ",
                                "Maker  Info  ",
                                };
    u8 num_flag=1;
    u8 key_val=0;                   //红外键值
    u8 meanu_row=0;                 //在菜单选项中所在行标

    show_meanu(2,meanu);
    while(num_flag)
    {
        key_val=Remote_Scan();
        switch(key_val)//显示菜单
        {
            case 98:num_flag=0;meanu_row=0;break;//ch	   取消键
            case 168:meanu_row=show_meanu(1,meanu);break;//+	下一行
            case 226:meanu_row=show_meanu(1,meanu);break;//ch+
            case 2:meanu_row=show_meanu(1,meanu);break;//>>|
            case 194:num_flag=0;break;//>||	  确定键
            case 34:meanu_row=show_meanu(0,meanu);break;//|<<		上一行
            case 224:meanu_row=show_meanu(0,meanu);break;//-
            case 162:meanu_row=show_meanu(0,meanu);break;//ch-
            default:num_flag=1;
        }
    }
    printf("option:%d\r\n",meanu_row);           //打印选中行在菜单选项中的排序
    switch(meanu_row)
    {
        case 0:*pmode=WorkMode;break;
        case 1:*pmode=AddUser;break;
        case 2:*pmode=DelUser;break;
        case 3:*pmode=RootLogin;break;
        case 4:*pmode=RootOut;break;
        case 5:*pmode=RootInit;break;
        case 6:*pmode=InitSystem;break;
        case 7:*pmode=SystemInfo;break;
        case 8:*pmode=MakerInfo;break;
        default:*pmode=WorkMode;
    }
}

/**
 * 信息页模式
 * @param ml 为0时显示系统信息页，为1时显示作者信息页
 */
void infomation_mode(u8 ml)
{
    char temp[15]={"User NUM:     "};		//用户个数
    char temp2[15]={"Root NUM:     "};		//管理员个数
    u8 t=11,num_flag=1,key=0;
    if(ml==0)						//系统信息页
    {
        LCD_clear();
        while(num_flag)
        {
            key=Remote_Scan();
            if(key!=0)
            {
                num_flag=0;			//按任意键退出此界面
            }
            LCD_write_english_string(0,0,"    Sys Info");

            //NUM_USER,48的ASCII码为数字0
            temp[t]=NUM_USER/100+48;					//取百位
            if(temp[t]==48) temp[t]=' ';
            temp[t+1]=NUM_USER/10%10+48;				//取十位
            if(temp[t+1]==48) temp[t+1]=' ';
            temp[t+2]=NUM_USER%10+48;				    //取个位
            LCD_write_english_string(0,1,temp);
            //NUM_ROOT
            temp2[t]=NUM_ROOT/100+48;
            if(temp2[t]==48) temp2[t]=' ';
            temp2[t+1]=NUM_ROOT/10%10+48;
            if(temp2[t+1]==48) temp2[t+1]=' ';
            temp2[t+2]=NUM_ROOT%10+48;
            LCD_write_english_string(0,2,temp2);
            LCD_write_english_string(0,3,"Data:2021-3-6");
            LCD_write_english_string(0,4,"Verson:V 2.0");
        }
    }
    else if(ml==1)					//作者信息页
    {
        LCD_clear();
        while(num_flag)
        {
            key=Remote_Scan();
            if(key!=0)
            {
                num_flag=0;			//按任意键退出此界面
            }
            LCD_write_english_string(0,0,"  Maker Info  ");
            LCD_write_english_string(0,1,"Maker:V1.0 Jakefish");
            LCD_write_english_string(0,2,"Maker:V1.1 LWJ");
            LCD_write_english_string(0,3,"Maker:V2.0 ZYQ");
            LCD_write_english_string(0,4,"It's finally done");
        }
    }
}
