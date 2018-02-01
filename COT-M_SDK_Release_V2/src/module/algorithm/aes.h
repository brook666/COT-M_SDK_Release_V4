#ifndef HEADER_AES_H
#define HEADER_AES_H

#include <stdint.h>


#define AES_MAXNR 14        // 最大圈数为14圈
#define AES_BLOCK_SIZE 16   // 每次加/解密拿出4*4＝16个字节

/* This should be a hidden type, but EVP requires that the size be known */
/* 密钥数据结构 */
struct aes_key_st {

    uint32_t rd_key[4 *(AES_MAXNR + 1)];   // 存储密钥数据

    uint32_t rounds; // 存储加/解密圈数
};
typedef struct aes_key_st AES_KEY;

const char *AES_options(void);

void EI_vAES_set_encrypt_key(const uint8_t *userKey, const int bits,AES_KEY *key);
void EI_vAES_set_decrypt_key(const uint8_t *userKey, const int bits,AES_KEY *key);

void EI_vAES_encrypt(const uint8_t *in, uint8_t *out,const AES_KEY *key);
void EI_vAES_decrypt(const uint8_t *in, uint8_t *out,const AES_KEY *key);


#endif

