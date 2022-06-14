CC=gcc
CCFLAGS=-fPIC -Wall -Wextra -g -02
LDFLAGS=-shared
TARGET_LIB=mv_multi.so

OBJECTS=mv_multi.c

.PHONY:all

$(TARGET_LIB):$(OBJECTS)
	$(CC) $(CCFLAGS) $(OBJECTS) ${LDFLAGS} -o $@ -lm

clean:
	-rm -f *.c *~
