/******************************************************************************
 * Project    : Smart Event Reminder and Digital Notice Board System
 * File       : main.c
 * Controller : LPC2129 (ARM7TDMI-S)
 * Author     : Surya Lakkineni
 *
 * Description:
 * ------------
 * This project implements a Smart Event Reminder and Digital Notice Board.
 *
 * Features:
 * ---------
 * 1. Displays current Time, Date and Day using RTC.
 * 2. Displays ambient temperature using LM35 sensor.
 * 3. Automatically displays scheduled event messages.
 * 4. Supports scrolling messages on 16x2 LCD.
 * 5. Provides a 15-minute countdown timer during events.
 * 6. Password-protected administration menu.
 * 7. Enable/Disable individual events.
 * 8. Red LED indicates Normal Mode.
 * 9. Green LED indicates Event Mode.
 * 10. Polling-based settings access.
 *
 ******************************************************************************/

/************************ HEADER FILES ************************/

#include <LPC21XX.h>      // LPC2129 register definitions
#include <string.h>       // String handling functions

#include "types.h"        // User defined data types
#include "delay.h"        // Delay functions
#include "lcd.h"          // LCD driver functions
#include "lcd_defines.h"  // LCD command definitions
#include "rtc.h"          // RTC driver functions
#include "adc.h"          // ADC driver functions
#include "kpm.h"          // Keypad driver functions
#include "menu.h"         // Settings menu functions

/************************ MACRO DEFINITIONS ************************/

#define TIME_LIMIT      15   // Event active duration in minutes
#define TOTAL_MESSAGES  10   // Total scheduled messages
#define LCD_WIDTH       16   // LCD display width

#define LED_RED         25   // Red LED connected to P1.25
#define LED_GREEN       26   // Green LED connected to P1.26

#define SETTINGS_SW     1    // Settings switch connected to P0.1

/************************ FUNCTION PROTOTYPES ************************/

void msg_scroll(const char *p,u32 size,u32 timems);
int find_strlen(const char *p);

/************************ GLOBAL VARIABLES ************************/

/* RTC Time Variables */
s32 hour,min,sec;

/* RTC Date Variables */
s32 date,month,year,day;

/* Event Start Time */
s32 startHour,startMin;

/* Event Control Flag */
u32 msg_flag = 1;

/************************ EVENT STRUCTURE ************************/

/*
 * Stores event schedule information.
 *
 * hour     -> Event trigger hour
 * minute   -> Event trigger minute
 * text     -> Event message
 * enabled  -> Enable/Disable flag
 */
typedef struct
{
    u8 hour;
    u8 minute;
    char text[80];
    u8 enabled;
}Message;

/************************ EVENT DATABASE ************************/

/*
 * Predefined event schedule table.
 *
 * Format:
 * {hour, minute, message, enabled}
 */
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

/******************************************************************************
 * Function Name : main
 * Description   : Main application entry point.
 *
 * Operation:
 * ----------
 * 1. Initialize peripherals.
 * 2. Display RTC and Temperature.
 * 3. Monitor settings switch.
 * 4. Check scheduled events.
 * 5. Execute Event Mode when event time matches.
 ******************************************************************************/
int main()
{
    s32 i;      // Event index
    u8 len;     // Message length

    /* Initialize peripherals */
    InitLCD();
    InitKPM();
    RTC_Init();
    Init_ADC();

    /* Configure LEDs as output */
    IODIR1 |= (1<<LED_RED) | (1<<LED_GREEN);

    /* Configure P0.1 as GPIO input */
    PINSEL0 &= ~(3<<2);
    IODIR0 &= ~(1<<SETTINGS_SW);

    /* System starts in Normal Mode */
    IOSET1 = (1<<LED_RED);
    IOCLR1 = (1<<LED_GREEN);

    while(1)
    {
        /**************** SETTINGS BUTTON POLLING ****************/

        if(!(IOPIN0 & (1<<SETTINGS_SW)))
        {
            delay_ms(20);    // Debounce delay

            if(!(IOPIN0 & (1<<SETTINGS_SW)))
            {
                settings();  // Open settings menu

                CmdLCD(CLEAR_LCD);

                while(!(IOPIN0 & (1<<SETTINGS_SW)));

                delay_ms(20);
            }
        }

        msg_flag = 1;

        /**************** NORMAL MODE INDICATION ****************/

        IOSET1 = (1<<LED_RED);
        IOCLR1 = (1<<LED_GREEN);

        /**************** READ RTC INFORMATION ****************/

        GetRTCTimeInfo(&hour,&min,&sec);
        GetRTCDateInfo(&date,&month,&year);
        GetRTCDay(&day);

        /**************** DISPLAY TIME AND DAY ****************/

        CmdLCD(GOTO_LINE1_POS0);
        DisplayRTCTime(hour,min,sec);
        CharLCD(' ');
        DisplayRTCDay(day);

        /**************** DISPLAY DATE ****************/

        CmdLCD(GOTO_LINE2_POS0);
        DisplayRTCDate(date,month,year);

        /**************** DISPLAY TEMPERATURE ****************/

        CmdLCD(GOTO_LINE2_POS0+11);
        U32LCD(Read_LM35());
        CharLCD(223);
        CharLCD('C');

        /**************** EVENT CHECKING ****************/

        for(i=0;i<TOTAL_MESSAGES;i++)
        {
            if(messageList[i].enabled &&
               messageList[i].hour == hour &&
               messageList[i].minute == min)
            {
                startHour = hour;
                startMin  = min;

                /* Enter Event Mode */
                IOCLR1 = (1<<LED_RED);
                IOSET1 = (1<<LED_GREEN);

                CmdLCD(CLEAR_LCD);

                while(1)
                {
                    GetRTCTimeInfo(&hour,&min,&sec);

                    /* Check event timeout */
                    if((((hour*60+min) -
                         (startHour*60+startMin)) > TIME_LIMIT) ||
                       (((hour*60+min) -
                         (startHour*60+startMin)) < 0))
                    {
                        break;
                    }

                    /* Calculate message length */
                    len = find_strlen(messageList[i].text);

                    /* Scroll message */
                    msg_scroll(messageList[i].text,len,200);

                    if(msg_flag == 0)
                        break;
                }

                /* Return to Normal Mode */
                CmdLCD(CLEAR_LCD);

                IOSET1 = (1<<LED_RED);
                IOCLR1 = (1<<LED_GREEN);
            }
        }
    }
}

/******************************************************************************
 * Function Name : msg_scroll
 * Description   : Scrolls event message on LCD.
 *
 * Parameters:
 *  p      -> Message string
 *  size   -> Message length
 *  timems -> Scroll delay
 ******************************************************************************/
void msg_scroll(const char *p,u32 size,u32 timems)
{
    char window[LCD_WIDTH+1];

    u32 i,j;

    s32 totalRemain;
    s32 remMin;
    s32 remSec;

    /* Create moving window for scrolling effect */

    for(i=0;i<=size;i++)
    {
        /* Copy 16 visible characters */
        for(j=0;j<LCD_WIDTH;j++)
        {
            if((i+j) < size)
                window[j] = p[i+j];
            else
                window[j] = ' ';
        }

        window[LCD_WIDTH] = '\0';

        /* Display scrolling message */
        CmdLCD(GOTO_LINE1_POS0);
        StrLCD(window);

        /* Read current time */
        GetRTCTimeInfo(&hour,&min,&sec);

        /* Calculate remaining event time */
        totalRemain =
        (TIME_LIMIT * 60) -
        (((hour * 60 + min) -
        (startHour * 60 + startMin)) * 60)
        - sec;

        if(totalRemain < 0)
            totalRemain = 0;

        remMin = totalRemain / 60;
        remSec = totalRemain % 60;

        /* Display countdown timer */
        CmdLCD(GOTO_LINE2_POS0);

        CharLCD((remMin/10)+'0');
        CharLCD((remMin%10)+'0');
        CharLCD(':');
        CharLCD((remSec/10)+'0');
        CharLCD((remSec%10)+'0');

        /* Display temperature */
        CmdLCD(GOTO_LINE2_POS0+11);

        U32LCD(Read_LM35());
        CharLCD(223);
        CharLCD('C');

        delay_ms(timems);

        if(msg_flag == 0)
            break;
    }
}

/******************************************************************************
 * Function Name : find_strlen
 * Description   : Calculates string length.
 *
 * Parameters:
 *  p -> Input string
 *
 * Return:
 *  Length of string
 ******************************************************************************/
int find_strlen(const char *p)
{
    int i = 0;

    while(p[i])
        i++;

    return i;
}
