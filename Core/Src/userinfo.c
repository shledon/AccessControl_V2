/* Includes ------------------------------------------------------------------*/
#include "userinfo.h"
#include "sys.h"
#include "w25qxx.h"
#include "retarget.h"
#include "display.h"
/* Private type ------------------------------------------------------------*/
struct USER student[USER_MAX_SIZE];//学生信息结构体

/* Private variables ---------------------------------------------------------*/
u16 NUM_USER=0;                      //记录学生总数
u8 NUM_ROOT=0;                      //记录管理员总数
u8 root[NUM_ROOT_MAX][4]={0};       //管理员信息数组

/**
 * 信息初始化
 * @param flag flag为1,则初始化全部，为0只初始化用户信息
 */
void information_init(u8 flag)
{
    u8 temp;
    u16 i,j;//i用来标记所有学生的偏移，j用来标记每个学生信息的偏移
    
    W25QXX_Read((u8*)&NUM_USER,FLASH_USER_NUM,1);//从第7M字节的地址开始存储信息
    printf("NUM_USER=%d\r\n",NUM_USER);//打印学生信息总数
    for(i=0;i<NUM_USER;i++)
    {
        for(j=0;j<USER_ID_SIZE;j++)//USER_ID_SIZE为4，这个循环从flash中读取信息填写STUDENT_ID
        {
            W25QXX_Read((u8*)&temp,FLASH_USER_DATA+i*SAVE_BLOG+j,1);
            student[i].id[j] = temp;
        }
        ShowID(student[i].id);//串口显示每个学生的ID
        
        for(j=0;j<USER_INFO_SIZE;j++)//USER_IFNO_SIZE为6,这个循环从flash中读取信息到student_info
        {
            W25QXX_Read((u8*)&temp,FLASH_USER_DATA+i*SAVE_BLOG+4+j,1);
            student[i].info[j] = temp;
        }
    }

    if(flag==INIT_ALL)
    {
        W25QXX_Read((u8*)&NUM_ROOT,FlASH_ROOT_NUM,1);
        printf("NUM_USER_ROOT=%d\r\n",NUM_ROOT);//打印管理员人数

        for(i=0;i<NUM_ROOT;i++)//这个循环从flash中读取信息填写root
        {
            for(j=0;j<USER_ID_SIZE;j++)
            {
                W25QXX_Read((u8*)&temp,FlASH_ROOT_DATA+i*4+j,1);
                root[i][j] = temp;
            }
            ShowID(root[i]);
        }
    }
}

/**
 * 更新FLASH中的信息
 */
void reflash_information()
{
    u16 i,j;
    u8 temp;
    W25QXX_Write((u8*)&NUM_USER,FLASH_USER_NUM,1);
    for(i=0;i<NUM_USER;i++)         //将STUDENT结构体中的信息写进FLASH中保存
    {
        for(j=0;j<USER_ID_SIZE;j++)
        {
            temp=student[i].id[j];
            W25QXX_Write(&temp,FLASH_USER_DATA+i*SAVE_BLOG+j,1);
        }

        for(j=0;j<USER_INFO_SIZE;j++)//
        {
            temp = student[i].info[j];
            W25QXX_Write(&temp,FLASH_USER_DATA+i*SAVE_BLOG+4+j,1);
        }
    }
}