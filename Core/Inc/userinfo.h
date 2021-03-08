#ifndef __USERINFO_H
#define __USERINFO_H 

#include "sys.h"


#define USER_ID_SIZE 4      //学生ID占据4个字节
#define USER_INFO_SIZE 6    //学生信息占据6个字节
#define USER_MAX_SIZE 100   //学生最大数目
#define USER_ID_LONG 12     //学生ID长度
#define SAVE_BLOG 10        //每个学生占据10个字节
#define PAGE_MAX 5          //每页最大行数
#define MEANU_MAX 4         //菜单每页最大行数
#define MEANU_LONG 9        //菜单选项个数
#define FLASH_SIZE 8*1024*1024;	                                        //FLASH大小为8M字节
#define FLASH_USER_NUM  8*1024*1024-1024*1024                           //用户信息从第7M处开始存储
#define FLASH_USER_DATA  8*1024*1024-1024*1024+4
#define NUM_USER_MAX 1000
#define NUM_ROOT_MAX 6
#define FlASH_ROOT_NUM  8*1024*1024-1024*1024+4+4*NUM_USER_MAX          //管理员信息存储在所有用户信息之后
#define FlASH_ROOT_DATA  8*1024*1024-1024*1024+4+4*NUM_USER_MAX+4
#define INIT_ALL 1                                                      //信息初始化标志

struct USER
{
	u8 id[USER_ID_SIZE];//4
	u8 info[USER_INFO_SIZE];//6
};

void information_init(u8 flag);
void reflash_information();

#endif

