#include <LPC21XX.h>
#include <string.h>

#include "types.h"
#include "delay.h"
#include "lcd.h"
#include "lcd_defines.h"
#include "rtc.h"
#include "adc.h"
#include "kpm.h"
#include "menu.h"

#define TIME_LIMIT      15
#define TOTAL_MESSAGES  10
#define LCD_WIDTH       16

#define LED_RED         25
#define LED_GREEN       26

#define SETTINGS_SW     1

void msg_scroll(const char *p,u32 size,u32 timems);
int find_strlen(const char *p);

s32 hour,min,sec;
s32 date,month,year,day;

s32 startHour,startMin;

u32 msg_flag = 1;

typedef struct
{
    u8 hour;
    u8 minute;
    char text[80];
    u8 enabled;
}Message;

Message messageList[TOTAL_MESSAGES] =
{
    {7,45 ,"               Good Morning! Classes Start Soon ",1},
    {9,45 ,"               ARM Workshop on external interrupts in LAB1 at 10AM ",1},
    {10,0 ,"               ARM kit issue time from 10AM - 10:30AM ",1},
    {10,15,"               C module lab exam in LAB2 ",1},
    {11,15,"               C module theory exam in LAB1 ",1},
    {12,45,"               Lunch Break from 1PM - 2PM ",1},
    {13,45,"               C Programming Session in Room 2 ",1},
    {15,15,"               Only 15 mins break time for next ARM session ",1},
    {17,0 ,"               Revise today's programs at home! ",1},
    {17,45,"               End of Day - See You Tomorrow! ",1}
};

int main()
{
    s32 i;
    u8 len;

    InitLCD();
    InitKPM();
    RTC_Init();
    Init_ADC();

    IODIR1 |= (1<<LED_RED) | (1<<LED_GREEN);

    /* P0.1 as GPIO input */
    PINSEL0 &= ~(3<<2);
    IODIR0  &= ~(1<<SETTINGS_SW);

    /* Normal Mode LED */
    IOSET1 = (1<<LED_RED);
    IOCLR1 = (1<<LED_GREEN);

    while(1)
    {
        /* SETTINGS BUTTON POLLING */
        if(!(IOPIN0 & (1<<SETTINGS_SW)))
        {
            delay_ms(20);

            if(!(IOPIN0 & (1<<SETTINGS_SW)))
            {
                settings();

                CmdLCD(CLEAR_LCD);

                while(!(IOPIN0 & (1<<SETTINGS_SW)));

                delay_ms(20);
            }
        }

        msg_flag = 1;

        /* NORMAL MODE LED */
        IOSET1 = (1<<LED_RED);
        IOCLR1 = (1<<LED_GREEN);

        GetRTCTimeInfo(&hour,&min,&sec);
        GetRTCDateInfo(&date,&month,&year);
        GetRTCDay(&day);

        /* LINE-1 */
        CmdLCD(GOTO_LINE1_POS0);
        DisplayRTCTime(hour,min,sec);
        CharLCD(' ');
        DisplayRTCDay(day);

        /* LINE-2 */
        CmdLCD(GOTO_LINE2_POS0);
        DisplayRTCDate(date,month,year);

        CmdLCD(GOTO_LINE2_POS0+11);
        U32LCD(Read_LM35());
        CharLCD(223);
        CharLCD('C');

        /* EVENT CHECK */
        for(i=0;i<TOTAL_MESSAGES;i++)
        {
            if(messageList[i].enabled &&
               messageList[i].hour == hour &&
               messageList[i].minute == min)
            {
                startHour = hour;
                startMin  = min;

                /* EVENT MODE LED */
                IOCLR1 = (1<<LED_RED);
                IOSET1 = (1<<LED_GREEN);

                CmdLCD(CLEAR_LCD);

                while(1)
                {
                    GetRTCTimeInfo(&hour,&min,&sec);

                    if((((hour*60+min) -
                         (startHour*60+startMin)) > TIME_LIMIT) ||
                       (((hour*60+min) -
                         (startHour*60+startMin)) < 0))
                    {
                        break;
                    }

                    len = find_strlen(messageList[i].text);

                    msg_scroll(messageList[i].text,len,200);

                    if(msg_flag == 0)
                        break;

                    /* SETTINGS BUTTON DURING EVENT MODE */
                    if(!(IOPIN0 & (1<<SETTINGS_SW)))
                    {
                        delay_ms(20);

                        if(!(IOPIN0 & (1<<SETTINGS_SW)))
                        {
                            settings();

                            CmdLCD(CLEAR_LCD);

                            while(!(IOPIN0 & (1<<SETTINGS_SW)));

                            delay_ms(20);
                        }
                    }
                }

                CmdLCD(CLEAR_LCD);

                /* BACK TO NORMAL MODE */
                IOSET1 = (1<<LED_RED);
                IOCLR1 = (1<<LED_GREEN);
            }
        }
    }
}

void msg_scroll(const char *p,u32 size,u32 timems)
{
    char window[LCD_WIDTH+1];
    u32 i,j;

    s32 totalRemain;
    s32 remMin;
    s32 remSec;

    for(i=0;i<=size;i++)
    {
        for(j=0;j<LCD_WIDTH;j++)
        {
            if((i+j) < size)
                window[j] = p[i+j];
            else
                window[j] = ' ';
        }

        window[LCD_WIDTH] = '\0';

        CmdLCD(GOTO_LINE1_POS0);
        StrLCD(window);

        GetRTCTimeInfo(&hour,&min,&sec);

        totalRemain =
        (TIME_LIMIT * 60) -
        (((hour * 60 + min) -
        (startHour * 60 + startMin)) * 60)
        - sec;

        if(totalRemain < 0)
            totalRemain = 0;

        remMin = totalRemain / 60;
        remSec = totalRemain % 60;

        CmdLCD(GOTO_LINE2_POS0);

        CharLCD((remMin/10)+'0');
        CharLCD((remMin%10)+'0');
        CharLCD(':');
        CharLCD((remSec/10)+'0');
        CharLCD((remSec%10)+'0');

        CmdLCD(GOTO_LINE2_POS0+11);

        U32LCD(Read_LM35());
        CharLCD(223);
        CharLCD('C');

        delay_ms(timems);

        if(msg_flag == 0)
            break;
    }
}

int find_strlen(const char *p)
{
    int i = 0;

    while(p[i])
        i++;

    return i;
}