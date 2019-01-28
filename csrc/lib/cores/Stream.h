#ifndef Stream_h
#define Stream_h

#include "inttypes.h"
#include <stdio.h>
#include <stdbool.h>

typedef enum{
    SKIP_ALL,
    SKIP_NONE,
    SKIP_WHITESPACE
}LookaheadMode;

#define NO_IGNORE_CHAR  '\x01'

typedef struct _MultiTarget {
  const char *str;
  size_t len;
  size_t index;
} MultiTarget;

//extern MultiTarget * multiTarget;

typedef struct Stream Stream;
struct Stream {
    unsigned long _timeout;
    unsigned long _startMillis;
    int (*available)(Stream *);
    int (*read)(Stream *);
    int (*peek)(Stream *);
};

void stream_init(struct Stream *stream);
int streamTimedRead(struct Stream *stream);
int streamTimedPeek(struct Stream *stream);
int streamPeekNextDigit(struct Stream *stream, LookaheadMode lookahead, bool detectDecimal);
void streamSetTimeout(struct Stream *stream, unsigned long timeout);
unsigned long streamGetTimeout(struct Stream *stream);
bool streamFind(struct Stream *stream, char *target);
bool streamFindLength(struct Stream *stream, char *target, size_t length);
bool streamFindUntil(struct Stream *stream, char *target, char *terminator);
bool streamFindUntilLength(struct Stream *stream, char *target, size_t targetLen, char *terminate, size_t termLen);
long streamParseInt(struct Stream *stream, LookaheadMode lookahead, char ignore);
float streamParseFloat(struct Stream *stream, LookaheadMode lookahead, char ignore);
size_t readBytes(struct Stream *stream, char *buffer, size_t length);
size_t readBytesUntil(struct Stream *stream, char terminator, char *buffer, size_t length);
int streamFindMulti(struct Stream *stream, MultiTarget *targets, int tCount);

#undef NO_IGNORE_CHAR
#endif
