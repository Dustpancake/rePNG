#include <stdio.h>
#include <string.h>

#include <repng.h>

int main(int argc, const char* argv[]) {
	/* entry point */
	int ret;
	FILE *fp;

	if (argc != 2) {
		printf("Requires a file as input.\n");
		return 1;
	}
	if (strstr(argv[1], ".png") == NULL ) {
		printf("Input file is not a .png\n");
		return 1;
	}

	fp = fopen(argv[1], "rb");
	if (read_header(fp) != 8) {
		printf("Header of image does not match .png format\n");
		return 1;
	}
	read_IDHR(fp);
	while ((ret = read_CHUNK(fp)) > 0) {
	}
	printf("\nRETCODE = %d\n", ret);
	fclose(fp);
	return 0;
}