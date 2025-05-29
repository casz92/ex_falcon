#include <stdlib.h>

static void *
xmalloc(size_t len)
{
    void *buf;

    if (len == 0)
    {
        return NULL;
    }
    buf = malloc(len);
    return buf;
}

static void
xfree(void *buf)
{
    if (buf != NULL)
    {
        free(buf);
    }
}

static char* 
bin2hex(const unsigned char *old, const size_t oldlen)
{
    char *result = (char*) malloc(oldlen * 2 + 1);
    size_t i, j;
    int b = 0;

    for (i = j = 0; i < oldlen; i++) {
        b = old[i] >> 4;
        result[j++] = (char) (87 + b + (((b - 10) >> 31) & -39));
        b = old[i] & 0xf;
        result[j++] = (char) (87 + b + (((b - 10) >> 31) & -39));
    }
    result[j] = '\0';
    return result;
}

// random bytes
#include <time.h>
void getRandom(uint8_t *buf, size_t len) {
    /* set the seed */
    srand( (unsigned)time( NULL ) );

    for (size_t i = 0; i < len; ++i) {
        buf[i] = rand();
    }
}