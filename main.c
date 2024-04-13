/*	P20 LED
	P24 ����
	P22 ����
	key1 ����1     �ֶ��Զ��л�����
	key5 LED����
	key9 ���ȿ���
	key13-16  �������1 2 3 4Ȧ
	key2  �޸�����
	key11  �˸�
	key12  ȷ���޸�����
	key13  �˳��޸�����
    P33  ��Ļ���⿪��



*/


#include <STC12C5A60S2.H>
#include "lcd12864.h"
#include "define.h"
#include "Matrixkey.h"
#include "Delay.h"
#include "AT24C02.h"
#include "DS1302.h"
#include "DHT11.h"
#include "GY30.h"
#include "Timer.h"
#include "stepmotor.h"
#include "Buzzer.h"




u16 Light;//���Ȳ���ֵ
bit flag, lcdflag = 0, ledflag, motorflag, Hflag, Lflag, autoflag;
/*
flag=0���������룬������ȷ�����flag=1
lcdflag=1����ʾʱ����ʪ�ȵ�״̬��ͨ����ʱ��1���
ledflag�ƹ��־λ
motorflag�����ȱ�־λ
hflag&lflag���Զ�����ʱ�����������־λ�жϵ�ǰ״̬�費��Ҫ�����������ı���޸���������־λ���´μ��ʱ�������û�иı�Ͳ��õ���
autoflag�Զ����ڱ�־λ������ı�һ�Σ�����ʪ�ȼ��ͬ��

*/
u8 stepmotorflag, laststep, changemode;
/*
stepmotorflag  �������״̬��1234Ϊλ��
laststep ���ڶ�ȡ��ǰ�������λ�ã���stepflag�Ƚ��������������Ҫִ�еĶ�������ֵ�洢��at24c02��5��
changemode  0�ֶ�������1�Զ�����
*/

void showmenu()				//��ʾ״̬���
{
    Lcd12864_ShowString(0, 0, "�¶�  �ȴ���:");
    Lcd12864_ShowString(1, 0, "ʪ��    ����:");
    Lcd12864_ShowString(2, 0, "����    �ƹ�: ");
    Lcd12864_ShowString(3, 0, "    ��  ��  ��");
    Lcd12864_ShowString(4, 0, "����    �ƹ�: ");
    Lcd12864_ShowString(6, 0,  "  :   :   ");
}

void autoset()			//����hflag&lflag�����Զ������������ֶ������л����Զ�����ʱhflag&lflag��׼ȷ
{
    if (DHTHotH <= 30)
    {
        motorflag = 0;
        stepmotorflag = 1;
        Hflag = 0;
    }
    else
    {
        motorflag = 1;
        stepmotorflag = 4;
        Hflag = 1;
    }

    if (Light <= 100)
    {
        ledflag = 1;
        stepmotorflag = 4;
        Lflag = 0;
    }					//����4��led��
    else
    {
        ledflag = 0;
        stepmotorflag = 1;
        Lflag = 1;
    }//����1,led��

    

    laststep = AT24C02_ReadByte(5);

    if (laststep != stepmotorflag)
    {
        EA = 0;											//Ϊ��ʹ������������������ر��ж�
        Lcd12864_Init();								//�ر��жϺ���ʾ��ˢ�£���ʾ���ڵ����������ٿ��ٸ�
        Lcd12864_ShowString(0, 0, "��\xfd�ڵ�������");
        step(stepmotorflag);
        showmenu();										//�ָ���ʾ
        EA = 1;
    }
	P20 = !ledflag;
    P22 = motorflag;
    autoflag = 0;
}
void main()
{
    u8 i, k = 0, KeyNum, err, keymode, temp[10];
    u32 Password, PasswordNum;
    P22 = 0;
    P33 = 1;
    EA = 1;
    Timer0_Init();
    TR0 = 0;
    Lcd12864_Init();
    Lcd12864_ShowString(0, 0, "znjjxt");
    Lcd12864_ShowString(1, 0, "0826");
    DS1302_Init();
    DS1302_SetTime();
    DHT_Init();
    GY30_Init();

    for (i = 0; i < 5; i++)
    {
        Delay(600);
        Right(1);
    }

    i = 0;
    Delay(600);
    ShutdownDisplay();
    TR0 = 1;
    P33 = 0;

    while (!KeyNum)
    {
        KeyNum = Key();
    }

    Lcd12864_Init();
    P33 = 1;
    Lcd12864_ShowString(0, 0, "�������룺");
    KeyNum = 0;
    PasswordNum = AT24C02_ReadByte(1) * 65536;
    PasswordNum += AT24C02_ReadByte(2) * 256;
    PasswordNum += AT24C02_ReadByte(3);
    //PasswordNum = 111111;

    while (1)
    {
        KeyNum = Key();

        if (flag == 0)
        {
            Delay(1);

            if (KeyNum <= 10 && KeyNum > 0)
            {
                if (i == 0)
                {
                    Lcd12864_WriteCMD(0x90);
                }

                Lcd12864_WriteData(0x2A);
                Password *= 10;
                Password += KeyNum % 10;
                i++;

                if (i == 6)
                {
                    if (Password == PasswordNum)
                    {
                        Password = 0;
                        Lcd12864_ShowString(1, 0, "������\xfdȷ    ");
                        P24 = 0;
                        flag = 1;
                        lcdflag = 1;
                        Delay(700);
                        showmenu();
                        stepmotorflag = AT24C02_ReadByte(5);
                        DS1302_ReadTime();
                        Timer1_Init();
                        P20 = 0;
                        Delay(100);
                        P20 = 1;
                        Delay(100);
                        P20 = 0;
                        Delay(100);
                        P20 = 1;
                        Delay(100);
                        P20 = 0;
                        Delay(100);
                        P20 = 1;
                    }
                    else
                    {
                        EA = 0;
                        err++;
                        Password = 0;
                        Lcd12864_ShowString(1, 0, "�������    ");
                        Lcd12864_ShowString(2, 0, "�������  ��");
                        Lcd12864_ShowNum(2, 4, err, 2);
                        i = 0;

                        if ((err) % 3 == 0)
                        {
                            Buzzerrun(500, 100);
                            Delay(100);
                            Buzzerrun(500, 25);
                            Delay(100);
                            Buzzerrun(500, 50);
                            Delay(5000);
                        }
                        else
                        {
                            Delay(1000);
                        }

                        Lcd12864_ShowString(1, 0, "            ");
                        EA = 1;
                    }
                }
            }
            else if (KeyNum == 11)
            {
                if (i > 0)
                {
                    i--;
                    Password /= 10;
                    Lcd12864_WriteCMD(0x90 + i / 2);

                    if (i % 2)
                    {
                        Lcd12864_WriteData(0x2A);
                    }

                    Lcd12864_WriteData(0x20);
                    Lcd12864_WriteCMD(0x90 + i / 2);

                    if (i % 2)
                    {
                        Lcd12864_WriteData(0x2A);
                    }
                }
            }

            KeyNum = 0;
        }
        else if (flag)
        {
            if (keymode == 0)
            {
                if (KeyNum == 1)
                {
                    if (changemode == 0)
                    {
                        autoset();
                    }

                    changemode = !changemode;
                }
                else if (KeyNum == 2)
                {
                    TR1 = 0;
                    Lcd12864_Init();
                    keymode = 1;
                    lcdflag = 0;
                }
                else
                {
                    DS1302_ReadTime();
                }

                if (changemode == 0)
                {
                    switch (KeyNum)
                    {
                    case 5:
                        ledflag = !ledflag;
                        P20 = !P20;
                        break;

                    case 9:
                        motorflag = !motorflag;
                        P22 = !P22;
                        break;

                    case 13:
                        stepmotorflag = 1;
                        EA = 0;
                        Lcd12864_Init();
                        Lcd12864_ShowString(0, 0, "��\xfd�ڵ�������");
                        step(1);
                        P20 = !ledflag;
                        showmenu();
                        EA = 1;
                        break;

                    case 14:
                        stepmotorflag = 2;
                        EA = 0;
                        Lcd12864_Init();
                        Lcd12864_ShowString(0, 0, "��\xfd�ڵ�������");
                        step(2);
                        P20 = !ledflag;
                        showmenu();
                        EA = 1;
                        break;

                    case 15:
                        stepmotorflag = 3;
                        EA = 0;
                        Lcd12864_Init();
                        Lcd12864_ShowString(0, 0, "��\xfd�ڵ�������");
                        step(3);
                        P20 = !ledflag;
                        showmenu();
                        EA = 1;
                        break;

                    case 16:
                        stepmotorflag = 4;
                        EA = 0;
                        Lcd12864_Init();
                        Lcd12864_ShowString(0, 0, "��\xfd�ڵ�������");
                        step(4);
                        P20 = !ledflag;
                        showmenu();
                        EA = 1;
                        break;

                    default:
                        break;
                    }
                }
                else
                {
                    if (autoflag)
                    {
                        if (DHTHotH <= 30)
                        {
                            if (Hflag)
                            {
                                motorflag = 0;
                                stepmotorflag = 1;
                                Hflag = 0;
                            }
                        }
                        else
                        {
                            if (Hflag == 0)
                            {
                                motorflag = 1;
                                stepmotorflag = 4;
                                Hflag = 1;
                            }
                        }

                        if (Light <= 100)
                        {
                            if (Lflag)
                            {
                                ledflag = 1;
                                stepmotorflag = 4;
                                Lflag = 0;
                            }
                        }					//����0��led��
                        else
                        {
                            if (Lflag == 0)
                            {
                                ledflag = 0;
                                stepmotorflag = 1;
                                Lflag = 1;
                            }
                        }//����1/4,led��

                        
                        
						laststep = AT24C02_ReadByte(5);
                        if (laststep != stepmotorflag)
                        {
                            EA = 0;
                            Lcd12864_Init();
                            Lcd12864_ShowString(0, 0, "��\xfd�ڵ�������");
                            step(stepmotorflag);
                            showmenu();
                            EA = 1;
                        }P20 = !ledflag;
P22 = motorflag;
                        autoflag = 0;
                    }
                }
            }
            else
            {
                Roll(0);
                Lcd12864_ShowString(0, 0, "�޸�����");

                if (KeyNum <= 10 && KeyNum > 0 && k < 6)
                {
                    if (k == 0)
                    {
                        Lcd12864_WriteCMD(0x90);
                    }

                    Password *= 10;
                    Password += KeyNum % 10;
                    temp[k] = KeyNum % 10 + 0x30;
                    Lcd12864_ShowString(1, 0,  "        ");
                    Lcd12864_ShowString(1, 0, temp);
                    k++;
                }

                if (KeyNum == 11)
                {
                    if (k > 0)
                    {
                        k--;
                        Password /= 10;
                        temp[k] = 0;
                        Lcd12864_ShowString(1, 0,  "        ");
                        Lcd12864_ShowString(1, 0, temp);
                    }
                }

                if (KeyNum == 12)
                {
                    AT24C02_WriteByte(1, Password / 65536 % 256);
                    Delay(5);
                    AT24C02_WriteByte(2, Password / 256 % 256);
                    Delay(5);
                    AT24C02_WriteByte(3, Password % 256);
                    Delay(5);
                    Lcd12864_ShowString(1, 0,  "        ");
                    Lcd12864_ShowString(1, 0,  "�޸ĳɹ�");
                    keymode = 0;
                    Delay(1000);
                    showmenu();
                    TR1 = 1;
                    lcdflag = 1;
                }

                if (KeyNum == 13)
                {
                    showmenu();
                    keymode = 0;
                    TR1 = 1;
                    lcdflag = 1;
                }
            }
        }
    }
}


void Timer0_Isr(void) interrupt 1
{
    static unsigned int T0Count, T4Count;
    TL0 = 0x66;				//���ö�ʱ��ʼֵ
    TH0 = 0xFC;				//���ö�ʱ��ʼֵ
    T0Count++;

    if (T0Count % 20 == 0)
    {
        Key_Loop();    //20ms����һ�ΰ�����������
    }
	T4Count %= 2000;

    if (T4Count == 0)
    {
        DHT();
        Light = Get_GY();
        autoflag = 1;
    }

    T4Count++;
}



void Timer1_Isr(void) interrupt 3
{
    static unsigned int T1Count, T2Count, T3Count;
    static unsigned char u, umode;
    TL1 = 0x66;				//���ö�ʱ��ʼֵ
    TH1 = 0xFC;				//���ö�ʱ��ʼֵ
    T1Count++;
    T2Count++;
    

    if (T3Count < 1600)
    {
        T3Count++;
    }

    if (T3Count == 1500)
    {
        P24 = 1;
    }

    if (T1Count >= 50)
    {
        T1Count = 0;

        if (umode == 0)
        {
            u++;
        }
        else
        {
            u--;
        }

        if (u >= 16 || u <= 0)
        {
            umode = !umode;
        }

        Roll(u);
    }

    if (T2Count == 20)
    {
        T2Count = 0;

        if (lcdflag == 1)
        {
            Lcd12864_ShowNum(3, 0, DS1302_Time[0] + 2000, 4);
            Lcd12864_ShowNum(3, 3, DS1302_Time[1], 2);
            Lcd12864_ShowNum(3, 5, DS1302_Time[2], 2);
            Lcd12864_ShowNum(6, 0, DS1302_Time[3], 2);
            Lcd12864_ShowNum(6, 2, DS1302_Time[4], 2);
            Lcd12864_ShowNum(6, 4, DS1302_Time[5], 2);
            Lcd12864_ShowNum(0, 2, DHTHotH, 2);
            Lcd12864_ShowNum(1, 2, DHTWetH, 2);
            Lcd12864_ShowNum(2, 2, Light, 4);
            Lcd12864_ShowNum(4, 2, Light, 4);

            if (ledflag)
            {
                Lcd12864_ShowString(2, 7, "��");//��
                Lcd12864_ShowString(4, 7, "��");
            }
            else
            {
                Lcd12864_ShowString(2, 7, "��");//��
                Lcd12864_ShowString(4, 7, "��");
            }

            if (motorflag)
            {
                Lcd12864_ShowString(1, 7, "��");//���ط���
            }
            else
            {
                Lcd12864_ShowString(1, 7, "��");    //���ط���
            }

            if (stepmotorflag == 4)
            {
                Lcd12864_ShowString(0, 6, ":1  ");    //����0
            }
            else if (stepmotorflag == 1)
            {
                Lcd12864_ShowString(0, 6, ":1/4");    //����1/4
            }
            else if (stepmotorflag == 2)
            {
                Lcd12864_ShowString(0, 6, ":1/2");    //����1/2
            }
            else if (stepmotorflag == 3)
            {
                Lcd12864_ShowString(0, 6, ":3/4");    //����3/4
            }

            if (changemode)
            {
                Lcd12864_ShowString(3, 7, "��");
            }
            else
            {
                Lcd12864_ShowString(3, 7, "��");    //�ֶ�����
            }
        }
    }
}
