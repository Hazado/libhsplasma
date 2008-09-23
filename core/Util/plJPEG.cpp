#include "plJPEG.h"
#include "Debug/plDebug.h"
#include "3rdPartyLibs/jpeg-6b/jinclude.h"
#include "3rdPartyLibs/jpeg-6b/jpeglib.h"
#include "3rdPartyLibs/jpeg-6b/jerror.h"

/* hsJPEGException */
hsJPEGException::hsJPEGException(const char* file, unsigned long line,
                                 const char* message) throw()
               : hsException(file, line) {
    fWhat = "libJPEG error";
    if (message != NULL)
        fWhat += plString(": ") + message;
}


extern "C" {

#define INPUT_BUF_SIZE  4096

/* hsStream JPEG source -- modelled after IJG's stdio src */
typedef struct {
    struct jpeg_source_mgr pub;
    hsStream* stream;
    JOCTET* buffer;
    boolean start_of_stream;
} jpeg_hsStream_source;

METHODDEF(void) init_hsStream_source(j_decompress_ptr dinfo) {
    jpeg_hsStream_source* src = (jpeg_hsStream_source*)dinfo->src;
    src->start_of_stream = TRUE;
}

METHODDEF(boolean) hsStream_fill_input_buffer(j_decompress_ptr dinfo) {
    jpeg_hsStream_source* src = (jpeg_hsStream_source*)dinfo->src;
    size_t nbytes;
    
    nbytes = src->stream->read(INPUT_BUF_SIZE, src->buffer);
    
    if (nbytes <= 0) {
        if (src->start_of_stream)
            ERREXIT(dinfo, JERR_INPUT_EMPTY);
        WARNMS(dinfo, JWRN_JPEG_EOF);

        /* Insert a fake EOI marker */
        src->buffer[0] = (JOCTET)0xFF;
        src->buffer[1] = (JOCTET)JPEG_EOI;
        nbytes = 2;
    }

    src->pub.next_input_byte = src->buffer;
    src->pub.bytes_in_buffer = nbytes;
    src->start_of_stream = FALSE;

    return TRUE;
}

METHODDEF(void) hsStream_skip_input_data(j_decompress_ptr dinfo, long num_bytes) {
    jpeg_hsStream_source* src = (jpeg_hsStream_source*)dinfo->src;

    if (num_bytes > 0) {
        src->stream->skip(num_bytes);
        src->pub.next_input_byte += (size_t)num_bytes;
        src->pub.bytes_in_buffer -= (size_t)num_bytes;
    }
}

METHODDEF(void) hsStream_term_source(j_decompress_ptr dinfo) { }

GLOBAL(void) jpeg_hsStream_src(j_decompress_ptr dinfo, hsStream* S) {
    jpeg_hsStream_source* src;

    if (dinfo->src == NULL) {
        dinfo->src = (struct jpeg_source_mgr*)
            (*dinfo->mem->alloc_small)((j_common_ptr)dinfo, JPOOL_PERMANENT,
				                       SIZEOF(jpeg_hsStream_source));
        src = (jpeg_hsStream_source*)dinfo->src;
        src->buffer = (JOCTET *)
            (*dinfo->mem->alloc_small)((j_common_ptr)dinfo, JPOOL_PERMANENT,
				                       INPUT_BUF_SIZE * SIZEOF(JOCTET));
    }

    src = (jpeg_hsStream_source*)dinfo->src;
    src->pub.init_source = init_hsStream_source;
    src->pub.fill_input_buffer = hsStream_fill_input_buffer;
    src->pub.skip_input_data = hsStream_skip_input_data;
    src->pub.resync_to_restart = jpeg_resync_to_restart;
    src->pub.term_source = hsStream_term_source;
    src->stream = S;
    src->pub.bytes_in_buffer = 0;
    src->pub.next_input_byte = NULL;
}

// JPEG error handler for libPlasma
METHODDEF(void) plasma_output_message(j_common_ptr cinfo) {
    char buffer[JMSG_LENGTH_MAX];
    (*cinfo->err->format_message)(cinfo, buffer);
    plDebug::Error(buffer);
}

METHODDEF(void) plasma_error_exit(j_common_ptr cinfo) {
    (*cinfo->err->output_message)(cinfo);
    jpeg_destroy(cinfo);
    throw hsJPEGException(__FILE__, __LINE__);
}

}


/* plJPEG */
plJPEG* plJPEG::s_inst = NULL;

plJPEG::plJPEG() {
    if (s_inst != NULL)
        throw hsJPEGException(__FILE__, __LINE__, "Never construct me more than once!");
    s_inst = this;

    cinfo.err = jpeg_std_error(&jerr);
    dinfo.err = cinfo.err;
    cinfo.err->error_exit = plasma_error_exit;
    cinfo.err->output_message = plasma_output_message;

    jpeg_create_compress(&cinfo);
    jpeg_create_decompress(&dinfo);
}

plJPEG::~plJPEG() {
    jpeg_destroy_compress(&cinfo);
    jpeg_destroy_decompress(&dinfo);

    s_inst = NULL;
}

void plJPEG::DecompressJPEG(hsStream* S, void* buf, size_t size) {
    JSAMPARRAY jbuffer;
    int row_stride;

    jpeg_hsStream_src(&dinfo, S);
    jpeg_read_header(&dinfo, TRUE);
    jpeg_start_decompress(&dinfo);

    row_stride = dinfo.output_width * dinfo.output_components;
    jbuffer = (*dinfo.mem->alloc_sarray)((j_common_ptr)&dinfo, JPOOL_IMAGE,
                                         row_stride, 1);

    size_t offs = 0;
    while (dinfo.output_scanline < dinfo.output_height) {
        if (offs + row_stride > size)
            throw hsJPEGException(__FILE__, __LINE__, "buffer overflow");
        jpeg_read_scanlines(&dinfo, jbuffer, 1);
        memcpy(((unsigned char*)buf) + offs, jbuffer[0], row_stride);
        offs += row_stride;
    }

    jpeg_finish_decompress(&dinfo);
}

void CompressJPEG(hsStream* S, void* buf, size_t size) {
    throw hsNotImplementedException(__FILE__, __LINE__);
}