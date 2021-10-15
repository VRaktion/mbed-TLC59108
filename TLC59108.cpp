#include "TLC59108.h"

TLC59108::TLC59108(PinName p_sda, PinName p_scl,
                   const uint8_t selectable_address)
    : addr(I2C_ADDR::BASE | selectable_address), i2c(new I2C(p_sda, p_scl)) {}

uint8_t TLC59108::init() {
  reset();
  wait_us(200000);
  // ThisThread::sleep_for(200);
  uint8_t status = 0;
  status &= this->setRegister(TLC59108::REGISTER::MODE1::ADDR,
                              TLC59108::REGISTER::MODE1::ALLCALL);
  status &=
      this->setLedOutputMode(TLC59108::LED_MODE::PWM_IND); // PWM_INDGRP);//
  status &= setBrightness((const uint8_t)0);
  return status;
}

uint8_t TLC59108::reset() {
  setLedOutputMode(LED_MODE::OFF);
  setRegister(REGISTER::MODE1::ADDR, REGISTER::MODE1::OSC_OFF);
  uint8_t status = 0;
  this->i2c->start();
  status &= this->i2c->write(0x96);
  status &= this->i2c->write(0xA5);
  status &= this->i2c->write(0x5A);
  this->i2c->stop();
  return status;
}

uint8_t TLC59108::setLedOutputMode(const uint8_t outputMode) {
  if (outputMode & 0xfc)
    return 0;

  uint8_t regValue =
      (outputMode << 6) | (outputMode << 4) | (outputMode << 2) | outputMode;

  uint8_t retVal = setRegister(TLC59108::REGISTER::LEDOUT0::ADDR, regValue);
  retVal &= setRegister(TLC59108::REGISTER::LEDOUT1::ADDR, regValue);
  return retVal;
}

uint8_t TLC59108::setBrightness(const uint8_t pwmChannel,
                                const uint8_t dutyCycle) {
  if (pwmChannel > 7)
    return 0; // ERROR::EINVAL;

  return setRegister(TLC59108::REGISTER::PWM0::ADDR + pwmChannel, dutyCycle);
}

uint8_t TLC59108::setBrightness(const uint8_t dutyCycle) {
  uint8_t dutyCycles[8] = {dutyCycle};
  return this->setBrightness(dutyCycles);
}

uint8_t TLC59108::setBrightness(const uint8_t dutyCycles[],
                                const uint8_t startChannel,
                                const uint8_t length) {
  if (startChannel + length <= NUM_CHANNELS) {
    return setRegisters(TLC59108::REGISTER::PWM0::ADDR + startChannel,
                        dutyCycles, length);
  } else {
    return 1;
  }
}

uint8_t TLC59108::setGroupBrightness(const uint8_t dutyCycle) {
  return setRegister(TLC59108::REGISTER::GRPPWM::ADDR, dutyCycle);
}

int TLC59108::setRegister(const uint8_t reg, const uint8_t value) {
  char cmd[2] = {reg, value};
  return this->i2c->write(addr, cmd, 2);
}

int TLC59108::setRegisters(const uint8_t reg, const uint8_t values[],
                           const uint8_t length) {
  uint8_t status = 0;

  char *buff = (char *)malloc(length + 1);
  *buff = reg | TLC59108::AUTO_INCREMENT::ALL;
  memcpy(buff + 1, values, length);
  status = this->i2c->write(addr, buff, length + 1, false);
  free(buff);
  return status;
}