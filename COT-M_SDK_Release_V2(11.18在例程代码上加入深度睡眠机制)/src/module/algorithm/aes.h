#ifndef HEADER_AES_H
#define HEADER_AES_H

#include <stdint.h>


#define AES_MAXNR 14        // ���Ȧ��Ϊ14Ȧ
#define AES_BLOCK_SIZE 16   // ÿ�μ�/�����ó�4*4��16���ֽ�

/* This should be a hidden type, but EVP requires that the size be known */
/* ��Կ���ݽṹ */
struct aes_key_st {

    uint32_t rd_key[4 *(AES_MAXNR + 1)];   // �洢��Կ����

    uint32_t rounds; // �洢��/����Ȧ��
};
typedef struct aes_key_st AES_KEY;

const char *AES_options(void);

void EI_vAES_set_encrypt_key(const uint8_t *userKey, const int bits,AES_KEY *key);
void EI_vAES_set_decrypt_key(const uint8_t *userKey, const int bits,AES_KEY *key);

void EI_vAES_encrypt(const uint8_t *in, uint8_t *out,const AES_KEY *key);
void EI_vAES_decrypt(const uint8_t *in, uint8_t *out,const AES_KEY *key);


#endif

