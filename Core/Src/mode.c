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
extern unsigned char SN[4]; //��ȡ���Ŀ���
extern u8 root[NUM_ROOT_MAX][4];//����Ա��Ϣ����

u8 get_root=0;
u8 get_super_root=0;
extern u8 NUM_ROOT;
extern u16 NUM_USER;

/**
 * ����������
 */
void initSN()
{
    SN[0]=0;
    SN[1]=0;
    SN[2]=0;
    SN[3]=0;
}

/**
 * ���µĹ���Ա����д��FLASH��ROOT����
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
 * ����ȡ�Ŀ��Ƿ�����һ�ζ�ȡ��һ��
 * @param data ����
 * @return
 */
u8 check_read_same()
{
    static u8 temp[4]={0};//ֻ��һ�γ�ֵ
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
 * ����û��Ƿ��Ѿ�����
 * @param data ����
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
 * ������Ա�Ƿ��Ѿ�����
 * @param data ����
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
 * ����û������û������Ϣ��д��STUDENT�ṹ��
 * @param idnum �û�ID
 */
u8 add_user(char idnum[])
{
    u16 i,j,flag=0;

    for(i=0;i<NUM_USER;i++)
    {
        flag=0;
        for(j=0;j<USER_INFO_SIZE;j++)                                  //��������ID�Ƿ��Ѿ�����
        {
            if(student[i].info[j]==(idnum[j*2]-48)+(idnum[j*2+1]-48)*10)//�������char����ת��������
            {
                flag++;
            }
        }
    }
    if(flag < 5)        //�����ID���ظ�����IDд��INFO��
    {
        for(i=0;i<USER_INFO_SIZE;i++)
        {
            student[NUM_USER].info[i]=(idnum[i*2]-48)+(idnum[i*2+1]-48)*10;//��2λ��ѹ��Ϊ1���ֽ�
        }
        for(i=0;i<USER_ID_SIZE;i++)
        {
            student[NUM_USER].id[i]=SN[i];
        }
        printf("\r\n%d %d %d %d\r\n",SN[0],SN[1],SN[2],SN[3]);
        printf("id=%d %d %d %d\r\n",student[NUM_USER].id[0],student[NUM_USER].id[1],student[NUM_USER].id[2],student[NUM_USER].id[3]);
        NUM_USER++;     //��ǰ�û�����һ
        for(i=0;i<11;i++)//�����Ͻ���������
        {
            idnum[i]=' ';
        }
    }
    else                //�����ID�ظ�
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
 * ��ӹ���Ա��������Ա�����Ϣ��д��FLASH��ROOT����
 */
u8 add_root()
{
    RC522_Handel();
    if(!check_read_same())            //�����ζ�ȡ�Ŀ��Ƿ������Ա��¼��ȡ�Ŀ�һ������һ��������ִ��
    {
        //���㿨δ���ڡ�����Ա��δ�ﵽ���������еǼ�
        if(!check_root_exist() && NUM_ROOT<=NUM_ROOT_MAX)
        {
            write_root_num();     //���µĹ���Ա����д��FLASH��ROOT����
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
 * ɾ���û������û������Ϣ��STUDENT�ṹ����ɾ��
 * @param cur Ҫɾ����ID������ID�е�����
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
 * ��ʾ����¼���ID
 * @param ml Ϊ1ʱ�����ƶ�1�У�Ϊ0ʱ�����ƶ�һ��
 * @return ���ص�ǰ�б�������ID�е�����
 */
u8 show_all_id(u8 ml)
{
    u8 i,j;
    static u8 base=0;                           //��ǰҳ��һ��������ID�е��б�
    static u8 cur=0;                            //�ڵ�ǰҳ�е��б�
    static char user_id[12]={"           "};


    if(ml==1)
    {
        if(cur==PAGE_MAX-1)                  //�б굽�ﵱǰҳ�����һ��
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
        if(cur==0)                           //�б굽�ﵱǰҳ�ĵ�һ��
        {
            if(base >0 )                        //��û��������ID�ĵ�һ��
            {
                base--;                         //����ǰҳ�ĵ�һ��������
            }
        }
        else
            cur--;
    }

    //��ʾ
    for(i=0;i<NUM_USER && i<PAGE_MAX-1;i++)
    {
        for(j=0;j<USER_INFO_SIZE;j++)
        {
            //��info�е�6���ֽ�ID��ѹΪ12���ֽ�
            user_id[j*2]=student[base+i].info[j]%10+48;      //ȡʮλ
            user_id[j*2+1]=student[base+i].info[j]/10+48;    //ȡ��λ
        }
        user_id[10]=' ';
        user_id[11]=' ';
        LCD_write_english_string(0,i,user_id);
    }
    //��ʾ��־'<'
    for(j=0;j<USER_INFO_SIZE;j++)
    {
        user_id[j*2]=student[base+cur].info[j]%10+48;
        user_id[j*2+1]=student[base+cur].info[j]/10+48;
    }
    user_id[10]=' ';
    user_id[11]='<';
    if(NUM_USER!=0) LCD_write_english_string(0,cur,user_id);

    return base+cur;             //���ص�ǰ�б�������ID�е�����
}

/**
 * ��ʾ�����ID
 * @param ml Ϊ1ʱд����һ�����֣�Ϊ0ʱɾ����һ������
 * @param num ����ң�ذ��µ����ְ���
 * @param idnum[] �û�ID
 */
u8 show_id(u8 ml,char num,char idnum[])
{
    static u8 cur=0;                            //�б�
    if(ml==1)
    {
        if(cur==USER_ID_LONG-1)         //��������������
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
 * ����ģʽ
 */
void read_mode()
{
    u16 i;
    RC522_Handel();   //�������������ؿ���SN[]
    for(i=0;i<NUM_USER;i++)//������ѧ����Ϣ�м����Ƿ���ƥ��
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
 * ����Աģʽ
 */
void root_mode()
{
    u8 i;
    u8 num_flag=1;
    u8 key_val=0;
    u8 super_root[4]={0x5A,0x4B,0x2F,0x15};

    RC522_Handel();
    if(!get_root)           //��û�õ�����ԱȨ�ޣ����й���Ա��¼
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
    else        //�ѵõ�����ԱȨ�ޣ����й���Ա����
    {
        while(num_flag)
        {
            key_val=Remote_Scan();
            if(!key_val)        //���ⰴ�������˳�
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
 * ����ģʽ
 */
void add_mode()
{
    u8 num_flag=1;
    u8 key_val=0;
    char idnum[11]={"          "};       //���������û�ID

    if(get_root)
    {
        RC522_Handel();//����
        if( !check_read_same() )//�鿴�Ƿ�Ϊͬһ�ſ�
        {
            if(!check_user_exist() && NUM_USER<=NUM_USER_MAX)
            {
                LCD_clear();
                LCD_write_english_string(0,0,"New Card!");//�ҵ�
                LCD_write_english_string(0,1,"Enter ID:");//�ҵ�
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
                        case 144:show_id(0,48,idnum);break;//EQɾ����һ������
                        case 98:num_flag=0;break;//ch   �˳����
                        case 194://>||  ȷ�����
                            if(add_user(idnum))
                            {
                                num_flag=0;
                                reflash_information();//����FLASH����
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
                        LCD_write_english_string(0,2,"   Unusual!");//�쳣���
                        delay_ms(1000);
                }
            }
        }
    }
}

/**
 * ɾ��ģʽ
 */
void del_mode()
{
    u8 key_val;             //��������ֵ
    u8 num_flag=1;
    u8 all_id_cur=0;        //�������Ѿ�¼���ID�е�����

    if(get_root)
    {
        if(NUM_USER!=0)            //�ִ��û�����Ϊ0
        {
            LCD_clear();
            show_all_id(2);
            while(num_flag)
            {
                key_val=Remote_Scan();
                switch(key_val)
                {
                    case 168:all_id_cur=show_all_id(1);break;//+ �����ƶ�һ��
                    case 224:all_id_cur=show_all_id(0);break;//- �����ƶ�һ��
                    case 98:num_flag=0;break;//ch �˳�ɾ��
                    case 194://>||   ȷ��ɾ��
                        if(del_user(all_id_cur))
                        {
                            num_flag=0;
                            reflash_information();//����FLASH����
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
 * �˵�ҳģʽ
 * @param pmode ѡ�еĲ˵��е�ѡ��
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
    u8 key_val=0;                   //�����ֵ
    u8 meanu_row=0;                 //�ڲ˵�ѡ���������б�

    show_meanu(2,meanu);
    while(num_flag)
    {
        key_val=Remote_Scan();
        switch(key_val)//��ʾ�˵�
        {
            case 98:num_flag=0;meanu_row=0;break;//ch	   ȡ����
            case 168:meanu_row=show_meanu(1,meanu);break;//+	��һ��
            case 226:meanu_row=show_meanu(1,meanu);break;//ch+
            case 2:meanu_row=show_meanu(1,meanu);break;//>>|
            case 194:num_flag=0;break;//>||	  ȷ����
            case 34:meanu_row=show_meanu(0,meanu);break;//|<<		��һ��
            case 224:meanu_row=show_meanu(0,meanu);break;//-
            case 162:meanu_row=show_meanu(0,meanu);break;//ch-
            default:num_flag=1;
        }
    }
    printf("option:%d\r\n",meanu_row);           //��ӡѡ�����ڲ˵�ѡ���е�����
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
 * ��Ϣҳģʽ
 * @param ml Ϊ0ʱ��ʾϵͳ��Ϣҳ��Ϊ1ʱ��ʾ������Ϣҳ
 */
void infomation_mode(u8 ml)
{
    char temp[15]={"User NUM:     "};		//�û�����
    char temp2[15]={"Root NUM:     "};		//����Ա����
    u8 t=11,num_flag=1,key=0;
    if(ml==0)						//ϵͳ��Ϣҳ
    {
        LCD_clear();
        while(num_flag)
        {
            key=Remote_Scan();
            if(key!=0)
            {
                num_flag=0;			//��������˳��˽���
            }
            LCD_write_english_string(0,0,"    Sys Info");

            //NUM_USER,48��ASCII��Ϊ����0
            temp[t]=NUM_USER/100+48;					//ȡ��λ
            if(temp[t]==48) temp[t]=' ';
            temp[t+1]=NUM_USER/10%10+48;				//ȡʮλ
            if(temp[t+1]==48) temp[t+1]=' ';
            temp[t+2]=NUM_USER%10+48;				    //ȡ��λ
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
    else if(ml==1)					//������Ϣҳ
    {
        LCD_clear();
        while(num_flag)
        {
            key=Remote_Scan();
            if(key!=0)
            {
                num_flag=0;			//��������˳��˽���
            }
            LCD_write_english_string(0,0,"  Maker Info  ");
            LCD_write_english_string(0,1,"Maker:V1.0 Jakefish");
            LCD_write_english_string(0,2,"Maker:V1.1 LWJ");
            LCD_write_english_string(0,3,"Maker:V2.0 ZYQ");
            LCD_write_english_string(0,4,"It's finally done");
        }
    }
}
