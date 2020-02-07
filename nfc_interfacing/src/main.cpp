/*
 * Copyright (c) 2014-2018 Cesanta Software Limited
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include<string.h>
#include "mgos.h"
#include "mgos_arduino_adafruit_pn532.h"

#define IRQ 13
#define RST 19

void read_rfid(int pin, void* arg) {
  LOG(LL_INFO, ("Interrupt received!!"));
  Adafruit_PN532* pn532 = (Adafruit_PN532*)arg;
  unsigned char uid[] = {0, 0, 0, 0, 0, 0, 0};
  unsigned char uidLength;

  int success = mgos_arduino_adafruit_pn532_readPassiveTargetID(pn532, PN532_MIFARE_ISO14443A, uid, &uidLength, 2000);
  if (success) {
    LOG(LL_INFO, ("Found an ISO14443A card with ID: %.*s", (int)uidLength, uid));
  }
}

enum mgos_app_init_result mgos_app_init(void) {
  Adafruit_PN532* pn532 = mgos_arduino_adafruit_pn532_create_i2c(IRQ, RST);
  int success = mgos_arduino_adafruit_pn532_begin(pn532);
  if (!success)
    return MGOS_APP_INIT_ERROR;

  int version = mgos_arduino_adafruit_pn532_getFirmwareVersion(pn532);
  if (!version) {
    LOG(LL_INFO, ("Didn't find PN53x board!"));
  }
  LOG(LL_INFO, ("Found CHIP PN5: %x", (version >> 24) & 0xFF));
  LOG(LL_INFO, ("Firmware ver. %d", (version >> 16) & 0xFF));

  if (!mgos_arduino_adafruit_pn532_SAMConfig(pn532)) {
    LOG(LL_INFO, ("SAM Config failed!"));
  }
  
  mgos_gpio_set_int_handler(IRQ, MGOS_GPIO_INT_EDGE_NEG, read_rfid, pn532);
  return MGOS_APP_INIT_SUCCESS;
}
