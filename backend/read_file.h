#ifndef READ
#define READ
#include <stdlib.h>

char *read_file(const char *filename)
{
	FILE* file = fopen(filename, "rb");

	if(!(file))
	{
		fprintf(stderr, "No such file %s", filename);
		exit(EXIT_FAILURE);
	}

	fseek(file, 0, SEEK_END);
	size_t size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if(size <= 0)
	{
		fclose(file);
		fprintf(stderr, "Invalid file size");
		exit(EXIT_FAILURE);
	}

	char *data = calloc(size, sizeof(*data));

	fread(data, size, 1, file);
	fclose(file);

	return data;
}

#endif
