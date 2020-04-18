#ifndef REPNG_H
#define REPNG_H

// custom data types
typedef unsigned char BYTE;

// macro functions
#define RBYTE(buffer, num, fp) fread(buffer, sizeof(BYTE), num, fp)
#define ERBYTE(buffer, num, fp) if (RBYTE(buffer, num, fp)<=0) goto ERROR

// inits
#define NEWCHUNK {"\0", 0, NULL, {0,0,0,0}}
#define NEWIDHR {"IDHR", 13, 0, 0, 0, 0, 0, 0, 0, {0,0,0,0}}
#define HEADER {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a}

// structs

struct IDHR {
	char name[5];
	int length;
	int width;
	int height;
	BYTE bd, ct, comp, fm, interlace;
	BYTE crc[4];
};

struct CHUNK {
	char name[5];
	int length;
	BYTE *content;
	BYTE crc[4];
};

// function declerations
void read_n_bytes(int n, FILE* fp, char string[]);
int read_header(FILE* fp);
int bytes_to_uint32(BYTE buf[4]);
int read_length(FILE* fp);
void read_type(FILE *fp);
void print_idhr(struct IDHR* chunk);
void print_chunk(struct CHUNK* chunk);
int read_IDHR(FILE* fp);
int read_CHUNK(FILE *fp);

#endif