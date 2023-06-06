#include <io.h>
#include <basetsd.h>

#include <getopt.h>

typedef SSIZE_T ssize_t;

#define close _close
#define write _write
#define mkstemp(s) _mktemp_s(s, strlen(s) + 1)
#define unlink _unlink
