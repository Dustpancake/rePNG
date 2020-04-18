#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <repng.h>

void read_n_bytes(int n, FILE* fp, char string[]) {
	printf("\nReading next %d bytes :: %s --\n", n, string);
	BYTE *buff;
	buff = malloc(sizeof(BYTE) * n);
	int maxi = fread(buff, sizeof(BYTE), n, fp);
	for (int i=0; i<maxi; i++) printf("%02x/%d  ", buff[i], buff[i]); 
	printf("\n");
	free(buff);
}

int read_header(FILE* fp) {
	printf("HEADER\n");
	BYTE header[8];
	BYTE exp[8] = HEADER;
	int i;
	if (RBYTE(header, 8, fp)) {
		for (i=0; i < 8; i++) {
			if (header[i] != exp[i]) goto EXIT;
			printf("%02x ", header[i]);
		}
	}
	
	EXIT:
	printf("\n");
	return i;
}

int bytes_to_uint32(BYTE buf[4]) {
	uint32_t num;
	// big endian
	num = buf[3] | (uint32_t)buf[2] << 8 | (uint32_t)buf[1] << 16 | (uint32_t)buf[0] << 24;
	return num;
}

int read_length(FILE* fp) {
	BYTE length[4];
	int val;
	printf("\nLENGTH\n");
	if (RBYTE(length, 4, fp)) {
		val = bytes_to_uint32(length);
		printf("%d\n", val);
	} else { val = -1; }
	return val;
}

void read_type(FILE *fp) {
	char type[5];
	if (RBYTE(type, 4, fp)) {
		type[4] = '\0';
	} else { type[0] = '\0'; }
	printf("\nTYPE\n%s\n", type);
}

void print_idhr(struct IDHR* chunk) {
	printf("\nCHUNK: %s\nlength\t\t%d\nwidth\t\t%d\nheight\t\t%d\nbit depth\t%02x\ncolour type\t%02x\ncompression\t%02x\nfilter method\t%02x\ninterlace\t%02x\ncrc32b\t\t", 
		chunk->name, 
		chunk->length, 
		chunk->width, 
		chunk->height, 
		chunk->bd, 
		chunk->ct, 
		chunk->comp, 
		chunk->fm, 
		chunk->interlace);
	for (int i=0; i < 4; i++) {
		printf("%02x ", chunk->crc[i]);
	}
	printf("\ncrc32\t\t%d\n", bytes_to_uint32(chunk->crc));
}

void print_chunk(struct CHUNK* chunk) {
	printf("\nCHUNK: %s\nlength\t\t%d\ncrc32b\t\t", chunk->name, chunk->length);
	for (int i=0; i < 4; i++) {
		printf("%02x ", chunk->crc[i]);
	}
	printf("\ncrc32\t\t%d\n", bytes_to_uint32(chunk->crc));
}

int read_IDHR(FILE* fp) {
	/* returns number of bytes to roll back to start of chunk */
	BYTE buf[17];
	struct IDHR idhr = NEWIDHR;
	if (!RBYTE(buf, 8, fp)) return -1;
	if (RBYTE(buf, 17, fp)) {
		idhr.width = bytes_to_uint32(buf);
		idhr.height = bytes_to_uint32(buf + (sizeof(BYTE) * 4));
		idhr.bd = *(buf + sizeof(BYTE) * 8);
		idhr.ct = *(buf + sizeof(BYTE) * 9);
		idhr.comp = *(buf + sizeof(BYTE) * 10);
		idhr.fm = *(buf + sizeof(BYTE) * 11);
		idhr.interlace = *(buf + sizeof(BYTE) * 12);
		for (int i = 0; i < 4; i++) {
			idhr.crc[i] = *(buf + sizeof(BYTE) * 13 + i);
		}
		print_idhr(&idhr);
	}
	return 13 + 12;
}

int read_CHUNK(FILE *fp) {
	struct CHUNK chunk = NEWCHUNK;
	int length;
	BYTE l_byte[4];
	char name[5];
	BYTE *content;

	ERBYTE(l_byte, 4, fp);
	length = bytes_to_uint32(l_byte);
	if (length < 0) goto ERROR;

	ERBYTE(name, 4, fp);
	name[4] = '\0';

	if (length == 0) goto NOCONTENT;

	content = malloc(sizeof(BYTE) * length);
	ERBYTE(content, length, fp);

	ERBYTE(l_byte, 4, fp);

	strcpy(chunk.name, name);
	chunk.length = length;
	chunk.content = content;
	for (int i = 0; i < 4; i++) {
		chunk.crc[i] = l_byte[i];
	}

	print_chunk(&chunk);

	free(content);
	return length + 12;
	ERROR: {
		if (content != NULL) free(content);
		return -1;
	}
	NOCONTENT: {
		strcpy(chunk.name, name);
		ERBYTE(l_byte, 4, fp);
		for (int i = 0; i < 4; i++) {
			chunk.crc[i] = l_byte[i];
		}
		print_chunk(&chunk);
		return 0;
	}
}