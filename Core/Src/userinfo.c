/* Includes ------------------------------------------------------------------*/
#include "userinfo.h"
#include "sys.h"
#include "w25qxx.h"
#include "retarget.h"
#include "display.h"
/* Private type ------------------------------------------------------------*/
struct USER student[USER_MAX_SIZE];//ѧ����Ϣ�ṹ��

/* Private variables ---------------------------------------------------------*/
u16 NUM_USER=0;                      //��¼ѧ������
u8 NUM_ROOT=0;                      //��¼����Ա����
u8 root[NUM_ROOT_MAX][4]={0};       //����Ա��Ϣ����

/**
 * ��Ϣ��ʼ��
 * @param flag flagΪ1,���ʼ��ȫ����Ϊ0ֻ��ʼ���û���Ϣ
 */
void information_init(u8 flag)
{
    u8 temp;
    u16 i,j;//i�����������ѧ����ƫ�ƣ�j�������ÿ��ѧ����Ϣ��ƫ��
    
    W25QXX_Read((u8*)&NUM_USER,FLASH_USER_NUM,1);//�ӵ�7M�ֽڵĵ�ַ��ʼ�洢��Ϣ
    printf("NUM_USER=%d\r\n",NUM_USER);//��ӡѧ����Ϣ����
    for(i=0;i<NUM_USER;i++)
    {
        for(j=0;j<USER_ID_SIZE;j++)//USER_ID_SIZEΪ4�����ѭ����flash�ж�ȡ��Ϣ��дSTUDENT_ID
        {
            W25QXX_Read((u8*)&temp,FLASH_USER_DATA+i*SAVE_BLOG+j,1);
            student[i].id[j] = temp;
        }
        ShowID(student[i].id);//������ʾÿ��ѧ����ID
        
        for(j=0;j<USER_INFO_SIZE;j++)//USER_IFNO_SIZEΪ6,���ѭ����flash�ж�ȡ��Ϣ��student_info
        {
            W25QXX_Read((u8*)&temp,FLASH_USER_DATA+i*SAVE_BLOG+4+j,1);
            student[i].info[j] = temp;
        }
    }

    if(flag==INIT_ALL)
    {
        W25QXX_Read((u8*)&NUM_ROOT,FlASH_ROOT_NUM,1);
        printf("NUM_USER_ROOT=%d\r\n",NUM_ROOT);//��ӡ����Ա����

        for(i=0;i<NUM_ROOT;i++)//���ѭ����flash�ж�ȡ��Ϣ��дroot
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
 * ����FLASH�е���Ϣ
 */
void reflash_information()
{
    u16 i,j;
    u8 temp;
    W25QXX_Write((u8*)&NUM_USER,FLASH_USER_NUM,1);
    for(i=0;i<NUM_USER;i++)         //��STUDENT�ṹ���е���Ϣд��FLASH�б���
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