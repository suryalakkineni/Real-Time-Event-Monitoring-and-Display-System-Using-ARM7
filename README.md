# Real-Time Event Monitoring and Display System Using ARM7

## Smart Event Reminder and Digital Notice Board System Using LPC2129

---

## Abstract

The Smart Event Reminder and Digital Notice Board System is an embedded application developed using the LPC2129 ARM7 microcontroller. The project is designed to automate the display of important announcements, reminders, examination schedules, workshop notifications, and daily events in educational institutions, offices, and industries.

The system continuously displays real-time information such as time, date, day, and temperature. When the current time matches a predefined event schedule, the system automatically enters Event Mode and displays the corresponding message as a scrolling notification on the LCD. A countdown timer indicates the remaining duration of the active event.

A password-protected administration menu allows authorized users to configure system time, date, day, and control individual event messages.

---

# Table of Contents

- Introduction
- Objectives
- Features
- Hardware Requirements
- Software Requirements
- System Architecture
- Project Flow
- Working Principle
- Normal Mode Operation
- Event Mode Operation
- Event Scheduling
- Administrator Menu
- Results
- Applications
- Future Enhancements
- Conclusion

---

# Introduction

In educational institutions, training centers, industries, and offices, important announcements are often communicated through traditional notice boards or verbal communication. These methods may lead to delayed information delivery and increased manual effort.

To overcome these limitations, this project implements a Smart Event Reminder and Digital Notice Board System using the LPC2129 ARM7 microcontroller. The system automatically displays scheduled event notifications at predefined times while continuously showing real-time clock and temperature information.

The project combines RTC interfacing, LCD display control, keypad-based administration, temperature sensing, event scheduling, and embedded software design into a single intelligent information display platform.

---

# Project Objectives

The primary objectives of this project are:

- Display real-time clock information.
- Monitor ambient temperature using LM35.
- Automatically display scheduled event messages.
- Provide countdown timer functionality.
- Allow administrators to modify system settings.
- Enable or disable individual events.
- Provide visual event indication using LEDs.
- Demonstrate real-time embedded event scheduling.

---

# Features

## Real-Time Clock Display

Displays:

- Hours
- Minutes
- Seconds
- Date
- Month
- Year
- Day

---

## Event Scheduling

- Supports 10 predefined events.
- Time-based automatic triggering.
- Individual event enable/disable control.

---

## Scrolling Message Display

- Supports long messages.
- Smooth scrolling mechanism.
- Dynamic LCD windowing.

---

## Countdown Timer

- Event duration: 15 minutes.
- Displays remaining event time in MM:SS format.

---

## Temperature Monitoring

- LM35 sensor interfacing.
- Real-time temperature display.

---

## Password Protection

Default Password:

```text
1234
```

Only authorized users can access administrative functions.

---

## LED Status Indicators

| LED | Function |
|------|-----------|
| Red LED | Normal Mode |
| Green LED | Event Mode |

---

# Hardware Requirements

## LPC2129 ARM7 Microcontroller

Features:

- ARM7TDMI-S Core
- 32-bit RISC Architecture
- On-Chip RTC
- ADC Module
- GPIO Support
- Low Power Consumption

---

## 16x2 LCD

Used to display:

- Time
- Date
- Day
- Temperature
- Event Messages
- Countdown Timer

---

## Matrix Keypad

Used for:

- Password Entry
- Menu Navigation
- System Configuration

---

## LM35 Temperature Sensor

Measures ambient temperature and provides analog output proportional to temperature.

Advantages:

- Linear Output
- High Accuracy
- Easy Interfacing
- No Calibration Required

---

## LEDs

### Red LED

Indicates Normal Mode.

### Green LED

Indicates Event Mode.

---

# Software Requirements

## Development Tools

- Keil µVision
- Flash Magic

## Programming Language

- Embedded C

## Header Files

```c
#include <LPC21xx.h>
#include "rtc.h"
#include "adc.h"
#include "lcd.h"
#include "delay.h"
#include "kpm.h"
#include "menu.h"
```

---

# System Architecture

```text
                  +----------------+
                  |    LPC2129     |
                  +--------+-------+
                           |
     ------------------------------------------------
     |              |             |                |
     |              |             |                |
   LCD            RTC         Keypad            LM35
     |              |             |                |
     ------------------------------------------------
                           |
                    Event Scheduler
                           |
                       LEDs
```

---

# Project Flow

```text
START
  |
  V
Initialize LCD
Initialize RTC
Initialize ADC
Initialize Keypad
Initialize GPIO
  |
  V
Display Time, Date,
Day and Temperature
  |
  V
Check Settings Switch
  |
  +----> Pressed?
  |          |
  |          V
  |     Password Check
  |          |
  |     Admin Menu
  |
  V
Check Event Schedule
  |
  +----> Event Matched?
             |
      +------+------+
      |             |
     NO            YES
      |             |
      |       Enter Event Mode
      |             |
      |      Scroll Message
      |             |
      |      Start Countdown
      |             |
      |      Display Temperature
      |             |
      |      Timer Expired?
      |             |
      +-------------+
             |
             V
      Return to Normal Mode
             |
             V
          Repeat
```

---

# Working Principle

After power-up, the microcontroller initializes:

- LCD
- RTC
- ADC
- Keypad
- GPIO

The system then continuously:

1. Reads current time from RTC.
2. Reads current date and day.
3. Reads temperature from LM35.
4. Displays information on LCD.
5. Monitors event schedules.
6. Checks the settings switch.

When a scheduled event time is reached, the system automatically enters Event Mode.

---

# Normal Mode Operation

LCD continuously displays:

### Line 1

```text
10:15:30 MON
```

### Line 2

```text
21/06/2026   31°C
```

LED Status:

```text
RED LED   = ON
GREEN LED = OFF
```

---

# Event Mode Operation

When the current time matches a scheduled event:

- LCD screen clears.
- Event message starts scrolling.
- Countdown timer begins.
- Green LED turns ON.
- Red LED turns OFF.

Example:

```text
ARM Workshop on External Interrupts in LAB1 at 10AM
```

---

# Countdown Timer

Each event remains active for:

```text
15 Minutes
```

Example:

```text
14:59
14:58
14:57
...
00:00
```

After timer expiration, the system returns to Normal Mode.

---

# Scheduled Events

| Event | Time | Description |
|---------|---------|-------------|
| 1 | 07:45 | Good Morning Message |
| 2 | 09:45 | ARM Workshop Notification |
| 3 | 10:00 | ARM Kit Issue Reminder |
| 4 | 10:15 | C Lab Examination |
| 5 | 11:15 | C Theory Examination |
| 6 | 12:45 | Lunch Break Reminder |
| 7 | 13:45 | C Programming Session |
| 8 | 15:15 | ARM Session Reminder |
| 9 | 17:00 | Daily Revision Reminder |
| 10 | 17:45 | End of Day Message |

---

# Administrator Menu

## Main Menu

```text
1. Time
2. Date
3. Day
4. Message Control
5. Exit
```

---

## Time Settings

```text
Hour   : 0 - 23
Minute : 0 - 59
Second : 0 - 59
```

---

## Date Settings

```text
Date  : 1 - 31
Month : 1 - 12
Year  : 2025 - 2999
```

---

## Day Settings

```text
0 = Sunday
1 = Monday
2 = Tuesday
3 = Wednesday
4 = Thursday
5 = Friday
6 = Saturday
```

---

## Message Management

Administrator can:

- Enable Messages
- Disable Messages

This allows selective activation of events.

---

# Results

The developed system successfully:

- Displays real-time clock information.
- Monitors temperature continuously.
- Automatically activates scheduled events.
- Displays scrolling notifications.
- Provides countdown timer functionality.
- Supports password-protected configuration.
- Allows event enable/disable control.
- Operates reliably without external supervision.

---

# Applications

## Educational Institutions

- Class schedules
- Examination reminders
- Workshop notifications

## Offices

- Meeting reminders
- Employee announcements

## Industries

- Shift change alerts
- Maintenance reminders

## Training Centers

- Session schedules
- Laboratory notifications

## Public Information Systems

- Digital notice boards
- Visitor information displays

---

# Future Enhancements

- EEPROM-based event storage
- GSM notification support
- CAN Bus networking
- IoT cloud integration
- SD Card event database
- Mobile application support
- Touchscreen interface
- Remote web configuration

---

# Conclusion

The Smart Event Reminder and Digital Notice Board System Using LPC2129 successfully demonstrates the integration of RTC, LCD, ADC, keypad, and scheduling algorithms in an embedded environment.

The project provides a reliable, automated, and user-friendly solution for displaying time-sensitive information and event notifications. It showcases important embedded concepts such as real-time scheduling, sensor interfacing, menu-driven configuration, and user interaction.

This system can serve as a foundation for advanced smart information management systems in educational institutions, industries, offices, and public information display applications.


---

## Author

**Surya Lakkineni**

Embedded Systems | ARM7 | LPC2129 | C Programming | Linux

Project Developed Using:
- LPC2129 ARM7 Microcontroller
- Embedded C
- Keil µVision
- Flash Magic

⭐ If you found this project useful, consider giving it a star on GitHub.
