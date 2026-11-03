#ifdef __LITEOS_A__
#define _BSD_SOURCE
#include<netdb.h>
#include<errno.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#endif
#include <netdb.h>
#include <unsupported_api.h>

#ifdef __LITEOS_A__
#include "lock.h"

#define SERV_FILE_PATH  "/etc/services"
#define BUFFER_SIZE     200
#define MAX_PORT        65535
#define MAX_ALIAS_NO    10

struct servdata
{
	FILE *servf;
	int stayopen;
	struct servent se;
	char buf[BUFFER_SIZE];
	char *alias_list[MAX_ALIAS_NO + 1];
};
static struct servdata *sd = NULL;
static int serv_lock[1];

static int servent_resolv()
{
	if (sd->buf[0] == '#')
		return -1;
	char *ch, *temp, *ptr = NULL;
	ch = strchr(sd->buf, '#');
	if (ch)
		*ch = '\0';
	ch = strtok_r(sd->buf, " \t\n", &ptr);
	temp = strtok_r(NULL, " \t\n", &ptr);
	if (!ch || !temp)
		return -1;
		
	sd->se.s_name = ch;
	ch = strchr(temp, '/');
	if (ch == NULL)
		return -1;
	*ch++ = '\0';
	sd->se.s_port = atoi(temp);
	if (sd->se.s_port < 1 || sd->se.s_port > MAX_PORT)
		return -1;
	sd->se.s_port = htons(sd->se.s_port);
	if (strcmp(ch, "tcp") != 0 && strcmp(ch, "udp") != 0)
		return -1;
	sd->se.s_proto = ch;

	int i = 0;
	while (i < MAX_ALIAS_NO) {
		ch = strtok_r(NULL, " \t\n", &ptr);
		sd->alias_list[i++] = ch;
	}
	sd->alias_list[MAX_ALIAS_NO] = NULL;
	sd->se.s_aliases = sd->alias_list;
	return 0;
}

static int fill_servent_buf(const char *name, const char *proto,
	int port, char **aliases_raw, int naliases,
	struct servent *se, char *buf, size_t buflen)
{
	size_t need = (naliases + 1) * sizeof(char *);
	need += strlen(name) + 1;
	need += strlen(proto) + 1;
	for (int i = 0; i < naliases; i++)
		need += strlen(aliases_raw[i]) + 1;
	need += sizeof(char *) - 1;

	if (buflen < need)
		return ERANGE;

	char *p = buf + (-(size_t)buf & (sizeof(char *) - 1));
	se->s_aliases = (char **)p;
	p += (naliases + 1) * sizeof(char *);

	se->s_name = p;
	(void)strcpy(p, name);
	p += strlen(name) + 1;

	se->s_port = htons((short)port);

	se->s_proto = p;
	(void)strcpy(p, proto);
	p += strlen(proto) + 1;

	for (int i = 0; i < naliases; i++) {
		se->s_aliases[i] = p;
		(void)strcpy(p, aliases_raw[i]);
		p += strlen(aliases_raw[i]) + 1;
	}
	se->s_aliases[naliases] = NULL;

	return 0;
}

static int parse_servent_r(char *line, struct servent *se,
	char *buf, size_t buflen)
{
	char *ch, *temp, *ptr = NULL;
	char *name, *proto;
	int port;
	char *aliases_raw[MAX_ALIAS_NO + 1];
	int naliases = 0;

	if (line[0] == '#')
		return ENOENT;

	ch = strchr(line, '#');
	if (ch)
		*ch = '\0';

	name = strtok_r(line, " \t\n", &ptr);
	temp = strtok_r(NULL, " \t\n", &ptr);
	if (!name || !temp)
		return ENOENT;

	ch = strchr(temp, '/');
	if (!ch)
		return ENOENT;
	*ch++ = '\0';

	char *endp;
	long port_val = strtol(temp, &endp, 10);
	if (*endp != '\0' || port_val < 1 || port_val > MAX_PORT)
		return ENOENT;
	port = (int)port_val;

	if (strcmp(ch, "tcp") != 0 && strcmp(ch, "udp") != 0)
		return ENOENT;
	proto = ch;

	while (naliases < MAX_ALIAS_NO) {
		ch = strtok_r(NULL, " \t\n", &ptr);
		if (!ch)
			break;
		aliases_raw[naliases] = ch;
		naliases++;
	}

	return fill_servent_buf(name, proto, port, aliases_raw, naliases,
		se, buf, buflen);
}
#endif

struct servent *getservent(void)
{
#ifdef __LITEOS_A__
	if (sd == NULL) {
		if ((sd = malloc(sizeof(struct servdata))) == NULL) {
			errno = ENOMEM;
			return NULL;
		}
		sd->servf = NULL;
		sd->stayopen = 0;
	}
	if (!sd->servf && !(sd->servf = fopen(SERV_FILE_PATH, "r"))) {
		errno = ENOENT;
		free(sd);
		sd = NULL;
		return NULL;
	}
	do {
		if (!fgets(sd->buf, BUFFER_SIZE - 1, sd->servf)) {
			errno = EINVAL;
			return NULL;
		}
	} while (servent_resolv());
	return &(sd->se);
#else
	return 0;
#endif
}

int getservent_r(struct servent *se, char *buf, size_t buflen,
	struct servent **result)
{
#ifdef __LITEOS_A__
	int rv;

	*result = NULL;
	LOCK(serv_lock);
	if (sd == NULL) {
		if ((sd = malloc(sizeof(struct servdata))) == NULL) {
			errno = ENOMEM;
			UNLOCK(serv_lock);
			return ENOMEM;
		}
		sd->servf = NULL;
		sd->stayopen = 0;
	}
	if (!sd->servf && !(sd->servf = fopen(SERV_FILE_PATH, "r"))) {
		free(sd);
		sd = NULL;
		errno = ENOENT;
		UNLOCK(serv_lock);
		return ENOENT;
	}

	for (;;) {
		long orig_offset = ftell(sd->servf);
		if (!fgets(sd->buf, BUFFER_SIZE - 1, sd->servf)) {
			rv = ferror(sd->servf) ? errno : ENOENT;
			errno = rv;
			UNLOCK(serv_lock);
			return rv;
		}
		rv = parse_servent_r(sd->buf, se, buf, buflen);
		if (rv == ENOENT) {
			continue;
		}
		if (rv == ERANGE) {
			if (orig_offset < 0 || fseek(sd->servf, orig_offset, SEEK_SET) != 0) {
				rv = ESPIPE;
			}
		}
		if (rv == 0) {
			*result = se;
		} else {
			errno = rv;
		}
		UNLOCK(serv_lock);
		return rv;
	}
#else
	(void)se;
	(void)buf;
	(void)buflen;
	if (result) {
		*result = NULL;
	}
	return 0;
#endif
}

void endservent(void)
{
#ifdef __LITEOS_A__
	if (sd == NULL)
		return;
	if (sd->stayopen == 0) {
		if (sd->servf != NULL)
			fclose(sd->servf);
		free(sd);
		sd = NULL;
	}
#endif
}

void setservent(int stayopen)
{
#ifdef __LITEOS_A__
	if (sd == NULL) {
		if ((sd = malloc(sizeof(struct servdata))) == NULL) {
			errno = ENOMEM;
			return;
		}
		sd->servf = NULL;
	}
	if (!sd->servf)
		sd->servf = fopen(SERV_FILE_PATH, "r");
	else
		rewind(sd->servf);
	sd->stayopen = stayopen;
#endif
}
