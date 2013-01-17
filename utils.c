/* The MIT License

   Copyright (c) 2008 Genome Research Ltd (GRL).

   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/

/* Contact: Heng Li <lh3@sanger.ac.uk> */
#define FSYNC_ON_FLUSH

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <zlib.h>
#include <errno.h>
#ifdef FSYNC_ON_FLUSH
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
#include "utils.h"


extern time_t _prog_start;

FILE *err_xopen_core(const char *func, const char *fn, const char *mode)
{
	FILE *fp = 0;
	if (strcmp(fn, "-") == 0)
		return (strstr(mode, "r"))? stdin : stdout;
	if ((fp = fopen(fn, mode)) == 0) {
		err_fatal(func, "fail to open file '%s' : %s", fn, strerror(errno));
	}
	// setvbuf(fp, NULL, _IOFBF, 1048576);
	return fp;
}
FILE *err_xreopen_core(const char *func, const char *fn, const char *mode, FILE *fp)
{
	if (freopen(fn, mode, fp) == 0) {
		err_fatal(func, "fail to open file '%s' : %s", fn, strerror(errno));
	}
	// setvbuf(fp, NULL, _IOFBF, 1048576);
	return fp;
}
gzFile err_xzopen_core(const char *func, const char *fn, const char *mode)
{
	gzFile fp;
	if (strcmp(fn, "-") == 0) {
		fp = gzdopen(fileno((strstr(mode, "r"))? stdin : stdout), mode);
		/* According to zlib.h, this is the only reason gzdopen can fail */
		if (!fp) err_fatal(func, "Out of memory");
		return fp;
	}
	if ((fp = gzopen(fn, mode)) == 0) {
		err_fatal(func, "fail to open file '%s' : %s", fn, errno ? strerror(errno) : "Out of memory");
	}
	// gzbuffer(fp, 524288);
	return fp;
}
void err_fatal(const char *header, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "[%s] ", header);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(EXIT_FAILURE);
}

void err_fatal_core(const char *header, const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "[%s] ", header);
	vfprintf(stderr, fmt, args);
	fprintf(stderr, " Abort!\n");
	va_end(args);
	abort();
}

void _err_fatal_simple(const char *func, const char *msg)
{
	fprintf(stderr, "[%s] %s\n", func, msg);
	exit(EXIT_FAILURE);
}

void _err_fatal_simple_core(const char *func, const char *msg)
{
	fprintf(stderr, "[%s] %s Abort!\n", func, msg);
	abort();
}
size_t err_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    size_t ret = fwrite(ptr, size, nmemb, stream);
    if (ret != nmemb) 
    {
        _err_fatal_simple("fwrite", strerror(errno));
    }
    return ret;
}

size_t err_fread_noeof(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t ret = fread(ptr, size, nmemb, stream);
	if (ret != nmemb)
	{
		_err_fatal_simple("fread", ferror(stream) ? strerror(errno) : "Unexpected end of file");
	}
	return ret;
}

int err_gzread(gzFile file, void *ptr, unsigned int len)
{
	int ret = gzread(file, ptr, len);

	if (ret < 0)
	{
		int errnum = 0;
		const char *msg = gzerror(file, &errnum);
		_err_fatal_simple("gzread", Z_ERRNO == errnum ? strerror(errno) : msg);
	}

	return ret;
}

int err_fseek(FILE *stream, long offset, int whence)
{
	int ret = fseek(stream, offset, whence);
	if (0 != ret)
	{
		_err_fatal_simple("fseek", strerror(errno));
	}
	return ret;
}

long err_ftell(FILE *stream)
{
	long ret = ftell(stream);
	if (-1 == ret)
	{
		_err_fatal_simple("ftell", strerror(errno));
	}
	return ret;
}

int err_printf(const char *format, ...) 
{
    va_list arg;
    int done;

    va_start(arg, format);
    done = vfprintf(stdout, format, arg);
    int saveErrno = errno;
    va_end(arg);

    if (done < 0) 
    {
        _err_fatal_simple("vfprintf(stdout)", strerror(saveErrno));
    }
    return done;
}

int err_fprintf(FILE *stream, const char *format, ...) 
{
    va_list arg;
    int done;

    va_start(arg, format);
    done = vfprintf(stream, format, arg);
    int saveErrno = errno;
    va_end(arg);

    if (done < 0) 
    {
        _err_fatal_simple("vfprintf", strerror(saveErrno));
    }
    return done;
}

int err_fputc(int c, FILE *stream)
{
	int ret = putc(c, stream);
	if (EOF == ret)
	{
		_err_fatal_simple("fputc", strerror(errno));
	}

	return ret;
}

int err_fputs(const char *s, FILE *stream)
{
	int ret = fputs(s, stream);
	if (EOF == ret)
	{
		_err_fatal_simple("fputs", strerror(errno));
	}

	return ret;
}

int err_fflush(FILE *stream) 
{
    int ret = fflush(stream);
    if (ret != 0) 
    {
        _err_fatal_simple("fflush", strerror(errno));
    }
#ifdef FSYNC_ON_FLUSH
	/* Calling fflush() ensures that all the data has made it to the
	   kernel buffers, but this may not be sufficient for remote filesystems
	   (e.g. NFS, lustre) as an error may still occur while the kernel
	   is copying the buffered data to the file server.  To be sure of
	   catching these errors, we need to call fsync() on the file
	   descriptor, but only if it is a regular file.  */
	{
		struct stat sbuf;
		if (0 != fstat(fileno(stream), &sbuf))
		{
			_err_fatal_simple("fstat", strerror(errno));
		}
		if (S_ISREG(sbuf.st_mode))
		{
			if (0 != fsync(fileno(stream)))
			{
				_err_fatal_simple("fsync", strerror(errno));
			}
		}
	}
#endif
    return ret;
}

int err_fclose(FILE *stream) 
{
    int ret = fclose(stream);
    if (ret != 0) 
    {
        _err_fatal_simple("fclose", strerror(errno));
    }
    return ret;
}

int err_gzclose(gzFile file)
{
	int ret = gzclose(file);
	if (Z_OK != ret)
	{
		_err_fatal_simple("gzclose", Z_ERRNO == ret ? strerror(errno) : zError(ret));
	}

	return ret;
}

void *err_calloc(size_t nmemb, size_t size, const char *file, unsigned int line, const char *func)
{
	void *p = calloc(nmemb, size);
	if (NULL == p)
	{
		err_fatal(func, "Failed to allocate %zd bytes at %s line %u: %s\n", nmemb * size, file, line, strerror(errno));
	}
	return p;
}

void *err_malloc(size_t size, const char *file, unsigned int line, const char *func)
{
	void *p = malloc(size);
	if (NULL == p)
	{
		err_fatal(func, "Failed to allocate %zd bytes at %s line %u: %s\n", size, file, line, strerror(errno));
	}
	return p;
}

void *err_realloc(void *ptr, size_t size, const char *file, unsigned int line, const char *func)
{
	void *p = realloc(ptr, size);
	if (NULL == p)
	{
		err_fatal(func, "Failed to allocate %zd bytes at %s line %u: %s\n", size, file, line, strerror(errno));
	}
	return p;
}

char *err_strdup(const char *s, const char *file, unsigned int line, const char *func)
{
	char *p = strdup(s);

	if (NULL == p)
	{
		err_fatal(func, "Failed to allocate %zd bytes at %s line %u: %s\n", strlen(s), file, line, strerror(errno));
	}
	return p;
}

clock_t clock(void)
{
	clock_t clks = 0;
	struct timeval st;
	time_t time_now;

	// mck - use wall time ...

	gettimeofday(&st, NULL);
	time_now = st.tv_sec * 1000000L + (time_t)st.tv_usec;
	clks = (clock_t)(((double)(time_now - _prog_start) / 1000000.0) * (double)CLOCKS_PER_SEC);

	return clks;
}

int getmaxrss(long *maxrsskb)
{
  int len = 0;
  int srtn = 0;
  char procf[257] = { "" };
  FILE *fp = NULL;
  char line[2001] = { "" };
  char crap[2001] = { "" };
  char units[2001] = { "" };
  long maxrss = 0L;

  if(maxrsskb == NULL){
    return -1;
  }

  sprintf(procf,"/proc/%d/status",getpid());

  fp = fopen(procf, "r");
  if(fp == NULL){
    return -1;
  }

  while(fgets(line, 2000, fp) != NULL){
    if(strncasecmp(line,"VmPeak:",7) == 0){
      len = (int)strlen(line);
      line[len-1] = '\0';
      srtn = sscanf(line,"%s%ld%s",crap,&maxrss,units);
      if(srtn == 2){
        *maxrsskb = maxrss / 1024L;
      }else if(srtn == 3){
        if( (strcasecmp(units,"B") == 0) || (strcasecmp(units,"BYTES") == 0) ){
          *maxrsskb = maxrss / 1024L;
        }else if( (strcasecmp(units,"k") == 0) || (strcasecmp(units,"kB") == 0) ){
          *maxrsskb = maxrss * 1L;
        }else if( (strcasecmp(units,"m") == 0) || (strcasecmp(units,"mB") == 0) ){
          *maxrsskb = maxrss * 1024L;
        }else if( (strcasecmp(units,"g") == 0) || (strcasecmp(units,"gB") == 0) ){
          *maxrsskb = maxrss * 1024L * 1024L;
        }else{
          *maxrsskb = maxrss * 1L;
        }
      }
      break;
    }
  }

  fclose(fp);
 
  return 0;
}
