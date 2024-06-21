// Copyright 2018 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <stdio.h>
#include <string.h>
#include "sha256.h"

#define ESP_GET_BE32(a) ((((unsigned int) (a)[0]) << 24) | (((unsigned int) (a)[1]) << 16) | \
             (((unsigned int) (a)[2]) << 8) | ((unsigned int) (a)[3]))

#define ESP_PUT_BE32(a, val)                    \
    do {                            \
        (a)[0] = (unsigned char) ((((unsigned int) (val)) >> 24) & 0xff);    \
        (a)[1] = (unsigned char) ((((unsigned int) (val)) >> 16) & 0xff);    \
        (a)[2] = (unsigned char) ((((unsigned int) (val)) >> 8) & 0xff);    \
        (a)[3] = (unsigned char) (((unsigned int) (val)) & 0xff);        \
    } while (0)

#define RORc(x, y) \
( ((((unsigned int) (x) & 0xFFFFFFFFUL) >> (unsigned int) ((y) & 31)) | \
   ((unsigned int) (x) << (unsigned int) (32 - ((y) & 31)))) & 0xFFFFFFFFUL)
#define Ch(x,y,z)       (z ^ (x & (y ^ z)))
#define Maj(x,y,z)      (((x | y) & z) | (x & y))
#define S(x, n)         RORc((x), (n))
#define R(x, n)         (((x)&0xFFFFFFFFUL)>>(n))
#define Sigma0(x)       (S(x, 2) ^ S(x, 13) ^ S(x, 22))
#define Sigma1(x)       (S(x, 6) ^ S(x, 11) ^ S(x, 25))
#define Gamma0(x)       (S(x, 7) ^ S(x, 18) ^ R(x, 3))
#define Gamma1(x)       (S(x, 17) ^ S(x, 19) ^ R(x, 10))
#ifndef MIN
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
#endif

#define RND(a,b,c,d,e,f,g,h,i)                          \
    t0 = h + Sigma1(e) + Ch(e, f, g) + K[i] + W[i];    \
    t1 = Sigma0(a) + Maj(a, b, c);            \
    d += t0;                    \
    h  = t0 + t1;

static const unsigned int K[64] = {
    0x428a2f98UL, 0x71374491UL, 0xb5c0fbcfUL, 0xe9b5dba5UL, 0x3956c25bUL,
    0x59f111f1UL, 0x923f82a4UL, 0xab1c5ed5UL, 0xd807aa98UL, 0x12835b01UL,
    0x243185beUL, 0x550c7dc3UL, 0x72be5d74UL, 0x80deb1feUL, 0x9bdc06a7UL,
    0xc19bf174UL, 0xe49b69c1UL, 0xefbe4786UL, 0x0fc19dc6UL, 0x240ca1ccUL,
    0x2de92c6fUL, 0x4a7484aaUL, 0x5cb0a9dcUL, 0x76f988daUL, 0x983e5152UL,
    0xa831c66dUL, 0xb00327c8UL, 0xbf597fc7UL, 0xc6e00bf3UL, 0xd5a79147UL,
    0x06ca6351UL, 0x14292967UL, 0x27b70a85UL, 0x2e1b2138UL, 0x4d2c6dfcUL,
    0x53380d13UL, 0x650a7354UL, 0x766a0abbUL, 0x81c2c92eUL, 0x92722c85UL,
    0xa2bfe8a1UL, 0xa81a664bUL, 0xc24b8b70UL, 0xc76c51a3UL, 0xd192e819UL,
    0xd6990624UL, 0xf40e3585UL, 0x106aa070UL, 0x19a4c116UL, 0x1e376c08UL,
    0x2748774cUL, 0x34b0bcb5UL, 0x391c0cb3UL, 0x4ed8aa4aUL, 0x5b9cca4fUL,
    0x682e6ff3UL, 0x748f82eeUL, 0x78a5636fUL, 0x84c87814UL, 0x8cc70208UL,
    0x90befffaUL, 0xa4506cebUL, 0xbef9a3f7UL, 0xc67178f2UL
};

static void esp_sha256_transform(esp_sha256_t *ctx, const unsigned char *buf)
{
    unsigned int S[8], W[64], t0, t1;
    unsigned int t;
    int i;

    for (i = 0; i < 8; i++)
        S[i] = ctx->state[i];

    for (i = 0; i < 16; i++)
        W[i] = ESP_GET_BE32(buf + (4 * i));

    for (i = 16; i < 64; i++)
        W[i] = Gamma1(W[i - 2]) + W[i - 7] + Gamma0(W[i - 15]) + W[i - 16];

    for (i = 0; i < 64; ++i) {
        RND(S[0], S[1], S[2], S[3], S[4], S[5], S[6], S[7], i);
        t = S[7]; S[7] = S[6]; S[6] = S[5]; S[5] = S[4];
        S[4] = S[3]; S[3] = S[2]; S[2] = S[1]; S[1] = S[0]; S[0] = t;
    }

    for (i = 0; i < 8; i++)
        ctx->state[i] = ctx->state[i] + S[i];
}

int esp_sha256_init(esp_sha256_t *ctx)
{
    ctx->total[0] = 0;
    ctx->total[1] = 0;

    ctx->state[0] = 0x6A09E667UL;
    ctx->state[1] = 0xBB67AE85UL;
    ctx->state[2] = 0x3C6EF372UL;
    ctx->state[3] = 0xA54FF53AUL;
    ctx->state[4] = 0x510E527FUL;
    ctx->state[5] = 0x9B05688CUL;
    ctx->state[6] = 0x1F83D9ABUL;
    ctx->state[7] = 0x5BE0CD19UL;

    return 0;
}

int esp_sha256_update(esp_sha256_t *ctx, const void *src, size_t size)
{
    size_t fill;
    unsigned int left;
    const unsigned char *input = (const unsigned char *)src;

    left = ctx->total[0] & 0x3F;
    fill = 64 - left;

    ctx->total[0] += size;
    ctx->total[0] &= 0xFFFFFFFF;

    if (ctx->total[0] < size)
        ctx->total[1]++;

    if (left && size >= fill) {
        memcpy(ctx->buffer + left, input, fill);

        esp_sha256_transform(ctx, ctx->buffer);

        input += fill;
        size  -= fill;
        left = 0;
    }

    while (size >= 64) {
        esp_sha256_transform(ctx, input);

        input += 64;
        size  -= 64;
    }

    if (size > 0)
        memcpy(ctx->buffer + left, input, size);

    return 0;
}

int esp_sha256_finish(esp_sha256_t *ctx)
{
    unsigned int used;
    unsigned int high, low;

    used = ctx->total[0] & 0x3F;

    ctx->buffer[used++] = 0x80;

    if (used <= 56) {
        memset(ctx->buffer + used, 0, 56 - used);
    } else {
        memset(ctx->buffer + used, 0, 64 - used);
        esp_sha256_transform(ctx, ctx->buffer);
        memset(ctx->buffer, 0, 56);
    }

    high = (ctx->total[0] >> 29) | (ctx->total[1] <<  3);
    low  = (ctx->total[0] <<  3);

    ESP_PUT_BE32(ctx->buffer +  56, high);
    ESP_PUT_BE32(ctx->buffer +  60, low);

    esp_sha256_transform(ctx, ctx->buffer);
/*
    ESP_PUT_BE32(out +  0, ctx->state[0]);
    ESP_PUT_BE32(out +  4, ctx->state[1]);
    ESP_PUT_BE32(out +  8, ctx->state[2]);
    ESP_PUT_BE32(out + 12, ctx->state[3]);
    ESP_PUT_BE32(out + 16, ctx->state[4]);
    ESP_PUT_BE32(out + 20, ctx->state[5]);
    ESP_PUT_BE32(out + 24, ctx->state[6]);
    ESP_PUT_BE32(out + 28, ctx->state[7]);
*/
    return( 0 );
}

