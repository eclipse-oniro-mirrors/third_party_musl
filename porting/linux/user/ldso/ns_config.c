/*-------------------------------------------------------------------------*/
/**
   @file    ns_config.c
   @author  yangwenjun
   @brief    deal with ld-musl-namespace.ini file
*/

#include <ctype.h>
#include <stdarg.h>
#include "ns_config.h"

/*---------------------------- Defines -------------------------------------*/
#define MAX_LINE_SIZE         (1024)
#define INI_INVALID_KEY     ((char*)-1)

typedef enum _line_status_ {
    LINE_UNPROCESSED,
    LINE_ERROR,
    LINE_EMPTY,
    LINE_COMMENT,
    LINE_SECTION,
    LINE_VALUE
} line_status;

#define MAX_KEY_LEN 256
static char g_key[MAX_KEY_LEN+1] = {0};

static char *config_key_join(const char *join, bool start)
{
    if (start) g_key[0] = 0;
    size_t cnt = MAX_KEY_LEN - strlen(g_key);
    return strncat(g_key, join, cnt);
}

static int default_error_callback(const char *format, ...)
{
    int ret = 0;
    va_list argptr;
    va_start(argptr, format);
    /* do not print
    ret = vfprintf(stderr, format, argptr);
    */
    va_end(argptr);
    return ret;
}

static int (*config_error_callback)(const char *, ...) = default_error_callback;

static void config_set_error_callback(int (*errback)(const char *, ...))
{
  if (errback) {
    config_error_callback = errback;
  } else {
    config_error_callback = default_error_callback;
  }
}

static line_status config_line(char *line, char *section, char *key, char *value)
{
    size_t len;
    char *split;

    if ((len = strcspn(line, "#;")) == 0) {
        /* comment line */
        return LINE_COMMENT;
    } 
    line[len] = 0;
    if ((len = strtrim(line)) == 0) {
        /* empty line */ 
         return LINE_EMPTY;
    } 
    if (line[0] == '[' && line[len-1] == ']') {
        /* section name */
        memcpy(section, line+1, len-2);
        section[len-2] = 0;
        strtrim(section);
        return LINE_SECTION;
    } 
    if (split = strchr(line, '=')) {
        /* key and value */
        size_t klen, vlen;
        klen = split - line;
        vlen = len - klen -1;
        if (klen > 0) memcpy(key, line, klen);
        if (vlen > 0) memcpy(value, split+1, vlen);
        key[klen] = 0;
        value[vlen] = 0;
        strtrim(key);
        strtrim(value);
        return LINE_VALUE;
    }
    return LINE_ERROR;
}

#define SECTION_DEFAULT_SIZE   16
#define KV_DEFAULT_SIZE   64

static kvlist * kvlist_alloc(size_t size)
{
    kvlist *kvs;

    if (size < KV_DEFAULT_SIZE) size = KV_DEFAULT_SIZE;

    kvs = (kvlist *)calloc(1, sizeof *kvs);
    if (kvs) {
        kvs->key = (char **)calloc(size, sizeof *kvs->key);
        kvs->val = (char **)calloc(size, sizeof *kvs->val);
        if (kvs->key && kvs->val) {
            kvs->size = size;
        } else {
            free(kvs->key);
            free(kvs->val);
            free(kvs);
            kvs = NULL;
        }
    }
    return kvs;
}

static void kvlist_realloc(kvlist *kvs)
{
    if (!kvs) return;
    size_t size = 2*kvs->size;
    if (size) {
        char **keys, **vals;
        keys = (char **)realloc(kvs->key, size * (sizeof *kvs->key));
        if (!keys) return;
        kvs->key = keys;
        vals = (char **)realloc(kvs->val, size * (sizeof *kvs->val));
        if (!vals) return;
        kvs->val = vals;
        kvs->size = size;
    }

    return;
}

static void kvlist_free(kvlist *kvs)
{
    size_t i;
    if (!kvs) return;
    for (i=0; i<kvs->num; i++) {
        free(kvs->key[i]);
        free(kvs->val[i]);
    }
    free(kvs->key);
    free(kvs->val);
    free(kvs);
}

static section_list *sections_alloc(size_t size)
{
    section_list *sections;
    if (size < SECTION_DEFAULT_SIZE) size = SECTION_DEFAULT_SIZE;

    sections = (section_list *)calloc(1, sizeof *sections);

    if (sections) {
        sections->names = (char**)calloc(size, sizeof *sections->names);
        sections->kvs = (kvlist**)calloc(size, sizeof *sections->kvs);
        if (sections->names && sections->kvs) {
            sections->size = size;
        } else {
            free(sections->names);
            free(sections->kvs);
            free(sections);
            sections = NULL;
        }
    }
    return sections;
}

static void sections_realloc(section_list *sections)
{
    if (!sections) return;
    size_t size = 2*sections->size;
    if (size) {
        char **names;
        kvlist **kvs;
        names = (char **)realloc(sections->names, size * (sizeof *sections->names));
        if (!names) return;
        sections->names = names;
        kvs = (kvlist **)realloc(sections->kvs, size * (sizeof *sections->kvs));
        if (!kvs) return;
        sections->kvs = kvs;
        sections->size = size;
    }
    return;
}

static void sections_free(section_list *sections)
{
    if (!sections) return;
    for (size_t i=0; i < sections->num; i++) {
        free(sections->names[i]);
        kvlist_free(sections->kvs[i]);
    }
    free(sections->names);
    free(sections->kvs);
    free(sections);
}

static void kvlist_set(kvlist *kvs, const char *key, const char *val)
{
    size_t i;
    if (!kvs||!key||!val) return;

    for (i=0; i < kvs->num; i++) {
        if (!strcmp(kvs->key[i], key)) {
            break;
        }
    }

    if (i < kvs->num) {
        char * v = strdup(val);
        if (v) {
            free(kvs->val[i]);
            kvs->val[i] = v;
        }
        return;
    }
    if (kvs->num == kvs->size) {
        kvlist_realloc(kvs);
    }
    if (kvs->num < kvs->size) {
        kvs->key[kvs->num] = strdup(key);
        kvs->val[kvs->num] = strdup(val);
        if (kvs->key[kvs->num] && kvs->val[kvs->num]) {
            kvs->num++;
        } else {
            free(kvs->key[kvs->num]);
            free(kvs->val[kvs->num]);
        }
    }
    return;
}

static void sections_set(section_list *sections, const char *name, const char *key, const char *val)
{
    kvlist* kvs = NULL;
    if (!sections||!name||!key||!val) return;

    for(size_t i=0; i < sections->num; i++) {
        if (!strcmp(sections->names[i], name)) {
            kvs = sections->kvs[i];
            break;
        }
    }
    if (kvs) {
        kvlist_set(kvs,key,val);
        return;
    }

    if (sections->num == sections->size) {
       sections_realloc(sections);
    }
    
    if (sections->num < sections->size) {
        kvs = kvlist_alloc(0);
        sections->names[sections->num] = strdup(name);
        sections->kvs[sections->num] = kvs;
        if (sections->names[sections->num] && kvs) {
            sections->num++;
            kvlist_set(kvs,key,val);
        } else {
            free(sections->names[sections->num]);
            kvlist_free(kvs);
        }
    }
}

static section_list *config_load(const char *filepath)
{
    FILE *file;
    char line[MAX_LINE_SIZE+1];
    char section[MAX_LINE_SIZE+1];
    char key[MAX_LINE_SIZE+1];
    char val[MAX_LINE_SIZE+1];

    size_t  len;
    int  lineno = 0;

    section_list *sections;

    if ((file = fopen(filepath, "r")) == NULL) {
        config_error_callback("config: cannot open %s\n", filepath);
        return NULL;
    }

    sections = sections_alloc(0);
    if (!sections) {
        fclose(file);
        return NULL;
    }

    memset(line, 0, sizeof line);
    memset(section, 0, sizeof section);
    memset(key, 0, sizeof key);
    memset(val, 0, sizeof val);

    while (fgets(line, sizeof line, file)) {
        lineno++;
        len = strlen(line);
        if (len == 0) continue;

        if (line[len-1]!='\n' && !feof(file)) {
            config_error_callback(
              "config: input line too long in %s (%d)\n", filepath, lineno);
            sections_free(sections);
            fclose(file);
            return NULL;
        }
       
        if (line[len-1] == '\n') {
            line[len-1] = 0;
            len--;
        }
       
        switch (config_line(line, section, key, val)) {
            case LINE_EMPTY:
            case LINE_COMMENT:
            case LINE_SECTION:
                break;
            case LINE_VALUE:
                sections_set(sections, section, key, val);
                break;
            case LINE_ERROR:
                config_error_callback(
                    "config: syntax error in %s (%d):\n-> %s\n",
                    filepath,
                    lineno,
                    line);
                break;
            default:
                break;
        }
    }
    fclose(file);
    return sections;
}

static ns_configor g_configor;

/* const define */
#define CONFIG_DEFAULT_FILE "/etc/ld-musl-namespace.ini"        /* default config file pathname */
#define SECTION_DIR_MAP "section.dir.map"   /* map of section and directory of app */
#define ATTR_NS_PREFIX "namespace"         /* prefix of namespace attribute */
#define ATTR_NS_LIB_PATHS "lib.paths"         /* library search paths */
#define ATTR_NS_PERMITTED_PATHS "permitted.paths"         /* when separated, permitted dir paths of libs, including sub dirs */
#define ATTR_NS_INHERITS "inherits"          /* inherited namespace */
#define ATTR_NS_SEPARATED "separated"         /* if separated */
#define ATTR_ADDED_NSLIST "added.nslist"      /* all namespace names except default */
#define ATTR_NS_DEFAULT "default"           /* default namespace name */
#define ATTR_NS_ALLOWED_LIBS "allowed.libs"      /* when separated, allowed library names */
#define ATTR_NS_INHERIT_SHARED_LIBS "shared.libs"      /* when inherited, shared library names */
/* get key-value list of section */
static kvlist *config_get_kvs(const char *sname)
{
    size_t  i;
    for (i=0; i<g_configor.sections->num; i++) {
        if (!strcmp(g_configor.sections->names[i], sname)) {
            return g_configor.sections->kvs[i];
        }
    }
    return NULL;
}
/* parse config, success 0, failure <0 */
static int config_parse(const char *file_path, const char *exe_path)
{
    kvlist* dirkvs;
    if (!exe_path) return -1;
    g_configor.exe_path = strdup(exe_path);
    const char * fpath = CONFIG_DEFAULT_FILE;
    if (file_path) fpath = file_path;
    g_configor.file_path = strdup(fpath);
    g_configor.sections = config_load(fpath);

    if (!g_configor.sections) {
        return -2;
    }
    dirkvs = config_get_kvs(SECTION_DIR_MAP);
    if (!dirkvs) return -3; /* no section directory map found */
    size_t i;
    char * sname = NULL;
    for (i=0; i<dirkvs->num; i++) {
       strlist * paths = strsplit(dirkvs->val[i], ":");
       if (paths) {
           size_t j;
           for (j=0; j<paths->num; j++) {
               if (!strcmp(paths->strs[j], exe_path)) break;
           }
           if (j<paths->num) sname = dirkvs->key[i];
        }
        strlist_free(paths);
        if (sname) break;
    }
    if (!sname)  return -4;/* no section found */
    if (!(g_configor.kvs = config_get_kvs(sname))) return -5;/* no section key-value list found */
    return 0;
}
/* get value by key */
static char *config_get_value(const char *key) 
{
    if (!g_configor.kvs) return NULL;
    size_t i;
    for (i=0; i<g_configor.kvs->num; i++) {
        if (!strcmp(g_configor.kvs->key[i], key)) return g_configor.kvs->val[i];
    }
    return NULL;
}
/* get namespace names except default */
static strlist *config_get_namespaces()
{
    char *key = config_key_join(ATTR_ADDED_NSLIST, true);
    char *val = config_get_value(key);
    return strsplit(val, ",");
}
/* get library search paths */
static char *config_get_lib_paths(const char *ns_name)
{
    config_key_join(ATTR_NS_PREFIX, true);
    config_key_join(".", false);
    config_key_join(ns_name, false);
    config_key_join(".", false);
    char *key = config_key_join(ATTR_NS_LIB_PATHS, false);
    return config_get_value(key);
}
/* get permitted paths */
static char *config_get_permitted_paths(const char *ns_name)
{
    config_key_join(ATTR_NS_PREFIX, true);
    config_key_join(".", false);
    config_key_join(ns_name, false);
    config_key_join(".", false);
    char *key = config_key_join(ATTR_NS_PERMITTED_PATHS, false);
    return config_get_value(key);
}
/* get inherited namespace names */
static strlist *config_get_inherits(const char *ns_name)
{
    config_key_join(ATTR_NS_PREFIX, true);
    config_key_join(".", false);
    config_key_join(ns_name, false);
    config_key_join(".", false);
    char *key = config_key_join(ATTR_NS_INHERITS, false);
    char *val = config_get_value(key);
    return strsplit(val, ",");
}
/* get separated */
static bool config_get_separated(const char *ns_name)
{
    config_key_join(ATTR_NS_PREFIX, true);
    config_key_join(".", false);
    config_key_join(ns_name, false);
    config_key_join(".", false);
    char *key = config_key_join(ATTR_NS_SEPARATED, false);
    char *val = config_get_value(key);
    strlwc(val);
    if (val && !strcmp("true", val)) return true;
    return false;  /* default false */
}
/* get allowed libs */
static char *config_get_allowed_libs(const char *ns_name)
{
    config_key_join(ATTR_NS_PREFIX, true);
    config_key_join(".", false);
    config_key_join(ns_name, false);
    config_key_join(".", false);
    char *key = config_key_join(ATTR_NS_ALLOWED_LIBS, false);
    return config_get_value(key);
}
/* get shared libs by inherited namespace */
static char *config_get_inherit_shared_libs(const char *ns_name, const char *inherited_ns_name)
{
    config_key_join(ATTR_NS_PREFIX, true);
    config_key_join(".", false);
    config_key_join(ns_name, false);
    config_key_join(".inherit.", false);
    config_key_join(inherited_ns_name, false);
    config_key_join(".", false);
    char *key = config_key_join(ATTR_NS_INHERIT_SHARED_LIBS, false);
    return config_get_value(key);
}

ns_configor *configor_init()
{
    memset(&g_configor, 0, sizeof g_configor);
    g_configor.set_error_callback = config_set_error_callback;
    g_configor.parse = config_parse;
    g_configor.get_namespaces = config_get_namespaces;
    g_configor.get_lib_paths = config_get_lib_paths;
    g_configor.get_permitted_paths = config_get_permitted_paths;
    g_configor.get_separated = config_get_separated;
    g_configor.get_inherits = config_get_inherits;
    g_configor.get_allowed_libs = config_get_allowed_libs;
    g_configor.get_inherit_shared_libs = config_get_inherit_shared_libs;
    return &g_configor;
}

void configor_free()
{
    if (g_configor.sections) {
        sections_free(g_configor.sections);
        g_configor.sections = NULL;
    }
    if (g_configor.file_path) {
        free(g_configor.file_path);
        g_configor.file_path = NULL;
    }
    if (g_configor.exe_path) {
        free(g_configor.exe_path);
        g_configor.exe_path = NULL;
    }
}