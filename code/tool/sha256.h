/*
 *  SHA-256 implementation with extra ESP8266 support added.
 *  Uses mbedTLS software implementation for failover when concurrent
 *  SHA operations are in use.
 *
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  Additions Copyright (C) 2016, Espressif Systems (Shanghai) PTE LTD
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */
#ifndef _SHA256_ALT_H_
#define _SHA256_ALT_H_

typedef struct
{
    unsigned int    total[2];
    unsigned int    state[8];
    unsigned char   buffer[64];

} esp_sha256_t;

int esp_sha256_init(esp_sha256_t *ctx);

int esp_sha256_update(esp_sha256_t *ctx, const void *src, size_t size);

int esp_sha256_finish(esp_sha256_t *ctx);


#endif
