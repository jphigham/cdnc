
#include "jpeg.h"

#include <stdio.h>
#include <setjmp.h>

#include <jpeglib.h>

jmp_buf setjmp_buffer;	/* for return to caller */

void my_error_exit (j_common_ptr cinfo)
{
  (*cinfo->err->output_message) (cinfo);
  longjmp(setjmp_buffer, 1);
}

int jpeg_read_mem(const unsigned char *jpeg_buffer, const unsigned long jpeg_size,
		unsigned int *width, unsigned int *height,
		unsigned char *data)
{
	struct jpeg_decompress_struct srcinfo;
	struct jpeg_error_mgr jsrcerr;

	srcinfo.err = jpeg_std_error(&jsrcerr);
	jsrcerr.error_exit = my_error_exit;

	if (setjmp(setjmp_buffer)) {
		jpeg_destroy_decompress(&srcinfo);
		return 1;
	}

	jpeg_create_decompress(&srcinfo);

    jpeg_mem_src(&srcinfo, jpeg_buffer, jpeg_size);

    jpeg_read_header(&srcinfo, TRUE);

    jpeg_start_decompress(&srcinfo);
    *width = srcinfo.output_width;
    *height = srcinfo.output_height;
	srcinfo.output_components = 4;
	srcinfo.out_color_space = JCS_EXT_RGBA;

    {
        JSAMPROW ptr = data;
        while (srcinfo.output_scanline < srcinfo.output_height)
        {
        	jpeg_read_scanlines(&srcinfo, &ptr, 1);
            ptr += srcinfo.output_width * srcinfo.output_components;
        }
    }

    jpeg_finish_decompress(&srcinfo);

    jpeg_destroy_decompress(&srcinfo);
	return 0;
}

int jpeg_read_file(const char *filename,
		unsigned int *width, unsigned int *height,
		unsigned char *data)
{
	struct jpeg_decompress_struct srcinfo;
	struct jpeg_error_mgr jsrcerr;

	FILE *fp;

	srcinfo.err = jpeg_std_error(&jsrcerr);
	jsrcerr.error_exit = my_error_exit;

	if (setjmp(setjmp_buffer)) {
		jpeg_destroy_decompress(&srcinfo);
		fclose(fp);
		return 1;
	}

	jpeg_create_decompress(&srcinfo);

	if ((fp = fopen(filename, "rb")) == NULL) {
      fprintf(stderr, "can't open %s for reading\n", filename);
      return 1;
    }

    jpeg_stdio_src(&srcinfo, fp);

    jpeg_read_header(&srcinfo, TRUE);

    jpeg_start_decompress(&srcinfo);
    *width = srcinfo.output_width;
    *height = srcinfo.output_height;
	srcinfo.output_components = 4;
	srcinfo.out_color_space = JCS_EXT_RGBA;

    {
        JSAMPROW ptr = data;
        while (srcinfo.output_scanline < srcinfo.output_height)
        {
        	jpeg_read_scanlines(&srcinfo, &ptr, 1);
            ptr += srcinfo.output_width * srcinfo.output_components;
        }
    }

    jpeg_finish_decompress(&srcinfo);

    fclose(fp);
    jpeg_destroy_decompress(&srcinfo);
	return 0;
}

