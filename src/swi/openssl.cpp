#include <stdint.h>
#include <swilib.h>
#include <swilib/openssl.h>

uint8_t *MD5_Hash(const uint8_t *data, unsigned long len, uint8_t *hash) {
	MD5_CTX ctx = {};
	MD5_Init(&ctx);
	MD5_Update(&ctx, data, len);
	MD5_Final(hash, &ctx);
	return hash;
}
