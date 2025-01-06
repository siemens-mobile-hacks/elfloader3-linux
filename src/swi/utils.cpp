#include <cstdlib>
#include <spdlog/spdlog.h>
#include <stdint.h>
#include <swilib.h>

static const char B64_ENCODE_TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static const uint8_t B64_DECODE_TABLE[] = {
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
	64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
	52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
	64, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
	15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
	64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64
};

void StrToHex(char *hex_out, const char *str, int len) {
	const char *hex = "0123456789ABCDEF";
	for (int i = 0; i < len; i++) {
		uint8_t c = (uint8_t) str[i];
		hex_out[i * 2] = hex[(c >> 4) & 0xF];
		hex_out[i * 2 + 1] = hex[c & 0xF];
	}
	hex_out[len * 2] = '\0';
}

int StrToInt(const char *s, char **endp) {
	return strtol(s, endp, 10);
}

int HexCharToInt(char c) {
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return -1;
}

void GetLangMesIntoWS(int lgp_id, WSHDR *str) {
	spdlog::debug("{}: not implemented!", __func__);
}

uint32_t HexToInt(const char *s) {
	return strtoul(s, NULL, 16);
}

int sdiv(int divisor, int divident) {
	return divident / divisor;
}

uint32_t udiv(uint32_t divisor, uint32_t divident) {
	return divident / divisor;
}

int DivBy10(int divident) {
	return divident / 10;
}

void SEQKILLER(void *data, void (*next_in_seq)(void *), void *data_to_kill) {
	next_in_seq(data);
	free(data_to_kill);
}

int Base64Encode(void *inbuf, int insize, void *outbuf, int outsize) {
	uint8_t *input = (uint8_t *) inbuf;
	char *output = (char *) outbuf;
	int enc_size = ((insize + 2) / 3) * 4;

	if (outsize < enc_size + 1) {
		return -1; // Output buffer too small
	}

	int i, j;
	for (i = 0, j = 0; i < insize;) {
		uint32_t octet_a = i < insize ? input[i++] : 0;
		uint32_t octet_b = i < insize ? input[i++] : 0;
		uint32_t octet_c = i < insize ? input[i++] : 0;

		uint32_t triple = (octet_a << 16) | (octet_b << 8) | octet_c;

		output[j++] = B64_ENCODE_TABLE[(triple >> 18) & 0x3F];
		output[j++] = B64_ENCODE_TABLE[(triple >> 12) & 0x3F];
		output[j++] = B64_ENCODE_TABLE[(triple >> 6) & 0x3F];
		output[j++] = B64_ENCODE_TABLE[triple & 0x3F];
	}

	for (i = 0; i < (3 - insize % 3) % 3; i++)
		output[enc_size - 1 - i] = '=';

	output[enc_size] = '\0';

	return enc_size;
}

int Base64Decode(char *inbuf, int insize, void *outbuf, int outsize, char *_null, int *unk5) {
	if (insize % 4 != 0)
		return -1;

	uint8_t *output = (uint8_t *) outbuf;
	int dec_size = insize / 4 * 3;
	if (inbuf[insize - 1] == '=') dec_size--;
	if (inbuf[insize - 2] == '=') dec_size--;

	if (outsize < dec_size)
		return -1;

	int i, j;
	for (i = 0, j = 0; i < insize;) {
		uint32_t sextet_a = inbuf[i] == '=' ? 0 & i++ : B64_DECODE_TABLE[(uint8_t) inbuf[i++]];
		uint32_t sextet_b = inbuf[i] == '=' ? 0 & i++ : B64_DECODE_TABLE[(uint8_t) inbuf[i++]];
		uint32_t sextet_c = inbuf[i] == '=' ? 0 & i++ : B64_DECODE_TABLE[(uint8_t) inbuf[i++]];
		uint32_t sextet_d = inbuf[i] == '=' ? 0 & i++ : B64_DECODE_TABLE[(uint8_t) inbuf[i++]];
		uint32_t triple = (sextet_a << 18) | (sextet_b << 12) | (sextet_c << 6) | sextet_d;
		if (j < dec_size)
			output[j++] = (triple >> 16) & 0xFF;
		if (j < dec_size)
			output[j++] = (triple >> 8) & 0xFF;
		if (j < dec_size)
			output[j++] = triple & 0xFF;
	}

	return dec_size;
}
