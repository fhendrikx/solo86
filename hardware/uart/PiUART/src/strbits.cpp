/*
  this file was derived from addons/linux/sprintf.cpp

  this file exists to supply a couple functions that libtelnet requires
*/

#include <circle/string.h>
#include <circle/util.h>

#ifdef __cplusplus
extern "C" {
#endif

int vsnprintf (char *buf, size_t size, const char *fmt, va_list var)
{
	CString Msg;
	Msg.FormatV (fmt, var);

	size_t len = Msg.GetLength ();
	if (--size < len)
	{
		len = size;
	}

	memcpy (buf, (const char *) Msg, len);
	buf[len] = '\0';

	return len;
}

char strerror_str[] = "ERROR";

char *strerror(int errnum) {

    return strerror_str;

}

#ifdef __cplusplus
}
#endif

