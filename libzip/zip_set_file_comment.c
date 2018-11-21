/*
  zip_set_file_comment.c -- set comment for file in archive
  Copyright (C) 2006-2012 Dieter Baron and Thomas Klausner

  This file is part of libzip, a library to manipulate ZIP archives.
  The authors can be contacted at <libzip@nih.at>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions
  are met:
  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in
     the documentation and/or other materials provided with the
     distribution.
  3. The names of the authors may not be used to endorse or promote
     products derived from this software without specific prior
     written permission.
 
  THIS SOFTWARE IS PROVIDED BY THE AUTHORS ``AS IS'' AND ANY EXPRESS
  OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
  GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
  IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
  IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/



#include <stdlib.h>
#include <string.h>

#include "zipint.h"



ZIP_EXTERN int
zip_set_file_comment(struct zip *za, zip_uint64_t idx,
		     const char *comment, int len)
{
    char *tmpcom;
    const char *name;
    enum zip_encoding_type com_enc, enc;

    if (idx >= za->nentry
	|| len < 0 || len > MAXCOMLEN
	|| (len > 0 && comment == NULL)) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return -1;
    }

    if (ZIP_IS_RDONLY(za)) {
	_zip_error_set(&za->error, ZIP_ER_RDONLY, 0);
	return -1;
    }

    if ((com_enc=_zip_guess_encoding(comment, len)) == ZIP_ENCODING_CP437) {
	_zip_error_set(&za->error, ZIP_ER_INVAL, 0);
	return -1;
    }

    if ((name=zip_get_name(za, idx, ZIP_FL_NAME_RAW)) == NULL)
	return -1;
    enc = _zip_guess_encoding(name, strlen(name));

    if (enc == ZIP_ENCODING_CP437 && com_enc == ZIP_ENCODING_UTF8) {
	_zip_error_set(&za->error, ZIP_ER_ENCMISMATCH, 0);
	return -1;
    }

    if (len > 0) {
	if ((tmpcom=(char *)_zip_memdup(comment, len, &za->error)) == NULL)
	    return -1;
    }
    else
	tmpcom = NULL;

    if (za->entry[idx].changes.valid & ZIP_DIRENT_COMMENT)
	free(za->entry[idx].changes.comment);
    za->entry[idx].changes.comment = tmpcom;
    za->entry[idx].changes.comment_len = len;
    za->entry[idx].changes.valid |= ZIP_DIRENT_COMMENT;
    
    return 0;
}