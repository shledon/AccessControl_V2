#ifndef __USERINFO_H
#define __USERINFO_H 

#include "sys.h"


#define USER_ID_SIZE 4      //ѧ��IDռ��4���ֽ�
#define USER_INFO_SIZE 6    //ѧ����Ϣռ��6���ֽ�
#define USER_MAX_SIZE 100   //ѧ�������Ŀ
#define USER_ID_LONG 12     //ѧ��ID����
#define SAVE_BLOG 10        //ÿ��ѧ��ռ��10���ֽ�
#define PAGE_MAX 5          //ÿҳ�������
#define MEANU_MAX 4         //�˵�ÿҳ�������
#define MEANU_LONG 9        //�˵�ѡ�����
#define FLASH_SIZE 8*1024*1024;	                                        //FLASH��СΪ8M�ֽ�
#define FLASH_USER_NUM  8*1024*1024-1024*1024                           //�û���Ϣ�ӵ�7M����ʼ�洢
#define FLASH_USER_DATA  8*1024*1024-1024*1024+4
#define NUM_USER_MAX 1000
#define NUM_ROOT_MAX 6
#define FlASH_ROOT_NUM  8*1024*1024-1024*1024+4+4*NUM_USER_MAX          //����Ա��Ϣ�洢�������û���Ϣ֮��
#define FlASH_ROOT_DATA  8*1024*1024-1024*1024+4+4*NUM_USER_MAX+4
#define INIT_ALL 1                                                      //��Ϣ��ʼ����־

struct USER
{
	u8 id[USER_ID_SIZE];//4
	u8 info[USER_INFO_SIZE];//6
};

void information_init(u8 flag);
void reflash_information();

#endif

