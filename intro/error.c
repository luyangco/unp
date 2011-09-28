#include <stdarg.h>
#include <syslog.h>

static void err_doit(int, int, const char *, va_list);

/* Fatal error related to a system call.
 * Print a message and terminate */
void
err_sys(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(1, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

/* Fatal error unrelated to a system call.
 * Print a message and terminate */
void
err_quit(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, LOG_ERR, fmt, ap);
	va_end(ap);
	exit(1);
}

/* Print a message and return to caller.
 * Caller specifies "errnoflag" and "level". */
static void
err_doit(int errnoflag, int level, const char *fmt, va_list ap)
{
	int errno_save, n;
	char buf[MAXLINE + 1];

	errno_save = errno; /* value caller must want printed */
#ifdef HAVE_VSNPRINTF
	vsnprintf(buf, MAXLINE, fmt, ap); /* this is safe */
#else
	vsprintf(buf, fmt, ap); /* this is not safe */
#endif
	n = strlen(buf);
	if (errnoflag)
		snprintf(buf + n, MAXLINE - n, ": %s", strerror(errno_value));
	strcat(buf, "\n");

	if (daemon_proc) {
		syslog(level, buf);
	} else {
		fflush(stdout); /* in case stdout and stderr are the same */
		fputs(buf, stderr);
		fflush(stderr);
	}
	return;
}
