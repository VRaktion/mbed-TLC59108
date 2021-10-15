/*
 * TLC59108: Arduino library to control TI TLC59108/TLC59108F/TLC59208 LED
 * drivers
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

class TLC59108 {
public:
  // default I2C addresses
  // datasheet, pp 12-13
  struct I2C_ADDR {
    static const uint8_t BASE = 0x80;
    static const uint8_t SWRESET = 0x4b;
    static const uint8_t ALLCALL = 0x90;
    static const uint8_t SUB1 = 0x92;
    static const uint8_t SUB2 = 0x94;
    static const uint8_t SUB3 = 0x98;
  };

  // register auto-increment modes for setting multiple registers
  // datasheet, p 13
  struct AUTO_INCREMENT {
    static const uint8_t ALL =
        0x80; // increment through all registers (for initial setup)
    static const uint8_t IND =
        0xa0; // increment through individual brightness registers
    static const uint8_t GLOBAL =
        0xc0; // increment through global control registers
    static const uint8_t INDGLOBAL =
        0xe0; // increment through individual and global registers
  };

  struct LED_MODE {
    static const uint8_t OFF = 0;
    static const uint8_t FULL_ON = 1;
    static const uint8_t PWM_IND = 2;
    static const uint8_t PWM_INDGRP = 3;
  };

  // register names
  // datasheet, p 16
  struct REGISTER {
  public:
    struct MODE1 {
      static const uint8_t ADDR = 0x00;

      static const uint8_t OSC_OFF = 0x10;
      static const uint8_t SUB1 = 0x08;
      static const uint8_t SUB2 = 0x04;
      static const uint8_t SUB3 = 0x02;
      static const uint8_t ALLCALL = 0x01;
    };

    struct MODE2 {
      static const uint8_t ADDR = 0x01;

      static const uint8_t EFCLR = 0x80;
      static const uint8_t DMBLNK = 0x20;
      static const uint8_t OCH = 0x08;
    };

    struct PWM0 {
      static const uint8_t ADDR = 0x02;
    };

    struct PWM1 {
      static const uint8_t ADDR = 0x03;
    };

    struct PWM2 {
      static const uint8_t ADDR = 0x04;
    };

    struct PWM3 {
      static const uint8_t ADDR = 0x05;
    };

    struct PWM4 {
      static const uint8_t ADDR = 0x06;
    };

    struct PWM5 {
      static const uint8_t ADDR = 0x07;
    };

    struct PWM6 {
      static const uint8_t ADDR = 0x08;
    };

    struct PWM7 {
      static const uint8_t ADDR = 0x09;
    };

    struct GRPPWM {
      static const uint8_t ADDR = 0x0a;
    };

    struct GRPFREQ {
      static const uint8_t ADDR = 0x0b;
    };

    struct LEDOUT0 {
      static const uint8_t ADDR = 0x0c;
    };

    struct LEDOUT1 {
      static const uint8_t ADDR = 0x0d;
    };

    struct SUBADR1 {
      static const uint8_t ADDR = 0x0e;
    };

    struct SUBADR2 {
      static const uint8_t ADDR = 0x0f;
    };

    struct SUBADR3 {
      static const uint8_t ADDR = 0x10;
    };

    struct ALLCALLADR {
      static const uint8_t ADDR = 0x11;
    };

    struct IREF {
      static const uint8_t ADDR = 0x12;

      static const uint8_t CM = 0x80; // current multiplier
      static const uint8_t HC = 0x40; // subcurrent
    };

    struct EFLAG {
      static const uint8_t ADDR = 0x13;
    };
  };

  TLC59108(PinName p_sda, PinName p_scl, const uint8_t selectable_address = 0);
  uint8_t init();
  uint8_t reset();
  uint8_t setLedOutputMode(const uint8_t outputMode);
  uint8_t setBrightness(const uint8_t pwmChannel, const uint8_t dutyCycle);
  uint8_t setBrightness(const uint8_t dutyCycle);
  uint8_t setBrightness(const uint8_t dutyCycles[],
                        const uint8_t startChannel = 0,
                        const uint8_t length = NUM_CHANNELS);
  uint8_t setGroupBrightness(const uint8_t dutyCycle);
  int setRegister(const uint8_t reg, const uint8_t value);
  int setRegisters(const uint8_t reg, const uint8_t values[],
                   const uint8_t length);

private:
  const static uint8_t NUM_CHANNELS = 8;
  uint8_t addr;
  I2C *i2c;
};

#endif