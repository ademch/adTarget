#include "stdafx.h"
#include <stdio.h>
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

char* read_shader_file(const char* filename)
{
size_t bytes;
struct _stat f_stat;
FILE *fp = NULL;
char str[500];

	if (!filename) return NULL;

	CharToOem( filename, &str[0] ); 
	printf("processing shader: %s\n", str);

	int fh = _open(filename, _O_RDONLY);
	if(fh >= 0)
	{
		int result = _fstat( fh, &f_stat );
		if( result != 0 )
		{
			fprintf( stderr, "An fstat error occurred.\n" );
			return NULL;
		}
		_close( fh );
	}
	else
	{
		fprintf(stderr,"Cannot open \"%s\" for read!\n", filename);
		return NULL;
	}

	long size = f_stat.st_size;

	char * buf = new char[size+1];	//plus trailing NULL

	fp = fopen(filename, "r");

	bytes = fread(buf, 1, size, fp);

	buf[bytes] = 0;

	fclose(fp);
return buf;
}
