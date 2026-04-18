#include "../../!!adGlobals/JPG/jpeg-9e/jpeglib.h"


struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr* my_error_ptr;

GLOBAL(int) read_JPEG_file(char* filename,
					/*out*/unsigned char** dataImg,
					/*out*/unsigned int& imageWidth,
					/*out*/unsigned int& imageHeight);
