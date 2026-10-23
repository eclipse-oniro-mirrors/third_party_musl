#include "pwf.h"
#include "lock.h"

static FILE *f;
static char *line;
static struct passwd pw;
static size_t size;
static int pwent_lock[1];

#define FIX(x) (pw->pw_##x = pw->pw_##x-rline+buf)

void setpwent()
{
	if (f) fclose(f);
	f = 0;
}

weak_alias(setpwent, endpwent);

struct passwd *getpwent()
{
	struct passwd *res;
	if (!f) f = fopen("/etc/passwd", "rbe");
	if (!f) return 0;
	__getpwent_a(f, &pw, &line, &size, &res);
	return res;
}

struct passwd *getpwuid(uid_t uid)
{
	struct passwd *res;
	__getpw_a(0, uid, &pw, &line, &size, &res);
	return res;
}

struct passwd *getpwnam(const char *name)
{
	struct passwd *res;
	__getpw_a(name, 0, &pw, &line, &size, &res);
	return res;
}

int getpwent_r(struct passwd *pw, char *buf, size_t size, struct passwd **res)
{
	char *rline = 0;
	size_t len = 0;
	struct passwd *tmp;
	int rv = 0;

	LOCK(pwent_lock);

	if (!f) {
		f = fopen("/etc/passwd", "rbe");
		if (!f) {
			rv = ENOENT;
			*res = 0;
			goto done;
		}
	}

	long orig_offset = ftell(f);
	rv = __getpwent_a(f, pw, &rline, &len, &tmp);
	if (!tmp) {
		*res = 0;
		if (rv == 0) {
			rv = ENOENT;
		}
		goto done;
	}
	if (size < len) {
		*res = 0;
		rv = ERANGE;
		free(rline);
		if (orig_offset < 0 || fseek(f, orig_offset, SEEK_SET) != 0) {
			rv = ESPIPE;
		}
		goto done;
	}
	memcpy(buf, rline, len);
	FIX(name);
	FIX(passwd);
	FIX(gecos);
	FIX(dir);
	FIX(shell);
	*res = pw;
	free(rline);
done:
	UNLOCK(pwent_lock);
	if (rv) {
		errno = rv;
	}
	return rv;
}
