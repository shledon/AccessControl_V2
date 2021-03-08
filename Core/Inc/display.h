#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"


#define	WorkMode  101
#define AddUser  102
#define DelUser  103
#define RootLogin  104
#define InitSystem 105
#define RootOut  106
#define MakerInfo 107
#define SystemInfo 108
#define RootInit 109


void showp1();
void showp2();
void ShowID(u8 *p);
u8 show_meanu(u8 ml,char show_char[][15]);
void execute(u8 key,u8 *blash);

#endif
