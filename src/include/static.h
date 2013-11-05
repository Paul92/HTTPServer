#ifndef STATIC_H
#define STATIC_H

#include "structs.h"

void staticHandler(struct headers*, int);
void sendHeaders(struct responseHeaders, int);
void sendFile(FILE*, int);


#endif
