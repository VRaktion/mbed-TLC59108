/*
 * TLC59108: Arduino library to control TI TLC59108/TLC59108F/TLC59208 LED drivers
 * 
 * (C) 2013 Christopher Smith <chrylis@gmail.com>
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __TLC59108_H__
#define __TLC59108_H__

#include "mbed.h"

class TLC59108
{
public:

    typedef uint8_t byte;

    // default I2C addresses
    // datasheet, pp 12-13
    struct I2C_ADDR
    {
        static const byte BASE = 0x80;
        static const byte SWRESET = 0x4b;
        static const byte ALLCALL = 0x90;
        static const byte SUB1 = 0x92;
        static const byte SUB2 = 0x94;
        static const byte SUB3 = 0x98;
    };

    // register auto-increment modes for setting multiple registers
    // datasheet, p 13
    struct AUTO_INCREMENT
    {
        static const byte ALL = 0x80; // increment through all registers (for initial setup)
        static const byte IND = 0xa0; // increment through individual brightness registers
        static const byte GLOBAL = 0xc0; // increment through global control registers
        static const byte INDGLOBAL = 0xe0; // increment through individual and global registers
    };

    struct LED_MODE
    {
        static const byte OFF = 0;
        static const byte FULL_ON = 1;
        static const byte PWM_IND = 2;
        static const byte PWM_INDGRP = 3;
    };

    // register names
    // datasheet, p 16
    struct REGISTER
    {
    public:
        struct MODE1
        {
            static const byte ADDR = 0x00;

            static const byte OSC_OFF = 0x10;
            static const byte SUB1 = 0x08;
            static const byte SUB2 = 0x04;
            static const byte SUB3 = 0x02;
            static const byte ALLCALL = 0x01;
        };

        struct MODE2
        {
            static const byte ADDR = 0x01;

            static const byte EFCLR = 0x80;
            static const byte DMBLNK = 0x20;
            static const byte OCH = 0x08;
        };

        struct PWM0
        {
            static const byte ADDR = 0x02;
        };

        struct PWM1
        {
            static const byte ADDR = 0x03;
        };

        struct PWM2
        {
            static const byte ADDR = 0x04;
        };

        struct PWM3
        {
            static const byte ADDR = 0x05;
        };

        struct PWM4
        {
            static const byte ADDR = 0x06;
        };

        struct PWM5
        {
            static const byte ADDR = 0x07;
        };

        struct PWM6
        {
            static const byte ADDR = 0x08;
        };

        struct PWM7
        {
            static const byte ADDR = 0x09;
        };

        struct GRPPWM
        {
            static const byte ADDR = 0x0a;
        };

        struct GRPFREQ
        {
            static const byte ADDR = 0x0b;
        };

        struct LEDOUT0
        {
            static const byte ADDR = 0x0c;
        };

        struct LEDOUT1
        {
            static const byte ADDR = 0x0d;
        };

        struct SUBADR1
        {
            static const byte ADDR = 0x0e;
        };

        struct SUBADR2
        {
            static const byte ADDR = 0x0f;
        };

        struct SUBADR3
        {
            static const byte ADDR = 0x10;
        };

        struct ALLCALLADR
        {
            static const byte ADDR = 0x11;
        };

        struct IREF
        {
            static const byte ADDR = 0x12;

            static const byte CM = 0x80; // current multiplier
            static const byte HC = 0x40; // subcurrent
        };

        struct EFLAG
        {
            static const byte ADDR = 0x13;
        };
    };

    // struct ERROR
    // {
    //     static const uint8_t EINVAL = 2;
    // };

    TLC59108(const PinName sda, const PinName scl, const byte selectable_address = 0):
        addr(I2C_ADDR::BASE | selectable_address),
        i2c(sda, scl) {
        setRegister(REGISTER::MODE1::ADDR, REGISTER::MODE1::ALLCALL);
    }

    uint8_t setLedOutputMode(const uint8_t outputMode)
    {
        if(outputMode & 0xfc)
            return 0;//ERROR::EINVAL;
        
        byte regValue = (outputMode << 6) | (outputMode << 4) | (outputMode << 2) | outputMode;
        
        uint8_t retVal = setRegister(REGISTER::LEDOUT0::ADDR, regValue);
        retVal &= setRegister(REGISTER::LEDOUT1::ADDR, regValue);
        return retVal;
    }

    uint8_t setBrightness(const uint8_t pwmChannel, const uint8_t dutyCycle)
    {
        if(pwmChannel > 7)
            return 0;//ERROR::EINVAL;
        
        return setRegister(REGISTER::PWM0::ADDR + pwmChannel, dutyCycle);
    }

    uint8_t setBrightness(const uint8_t dutyCycle)
    {
        uint8_t status = 0;
        i2c.start();
        status &= i2c.write(addr);
        status &= i2c.write(REGISTER::PWM0::ADDR | AUTO_INCREMENT::IND);
        for (uint8_t i = 0; i < NUM_CHANNELS; i++)
            status &= i2c.write(dutyCycle);
        i2c.stop();
        return status;
    }
    
    uint8_t setBrightness(const byte dutyCycles[], const uint8_t length = NUM_CHANNELS) 
    {
        return setRegisters(REGISTER::PWM0::ADDR, dutyCycles, length);
    }

    uint8_t setGroupBrightness(const uint8_t dutyCycle)
    {
        return setRegister(REGISTER::GRPPWM::ADDR, dutyCycle);
    }

    int setRegister(const byte reg, const byte value) {
        char cmd[2] = { reg, value };
        return i2c.write(addr, cmd, 2);
    }
    
    int setRegisters(const byte reg, const byte values[], const uint8_t length) {
        uint8_t status = 0;
        i2c.start();
        status &= i2c.write(addr);
        status &= i2c.write(reg | AUTO_INCREMENT::ALL);
        for (uint8_t i = 0; i < length; i++)
            status &= i2c.write(values[i]);
        i2c.stop();
        return status;
    }

protected:
    const static uint8_t NUM_CHANNELS = 8;
    byte addr;
    I2C i2c;
};

#endif