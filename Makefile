SOURCES = $(wildcard src/**/*.c src/*.c)
OBJECTS = $(patsubst %.c,%.o,$(SOURCES))

TARGET = setuid.so

LIBS =
CFLAGS = -O2 -shared -fPIC -std=c11 -D_GNU_SOURCE -Wall -Wextra -Wno-nonnull -Wwrite-strings -Wformat=2 -DNDEBUG

ifdef LUA_INCDIR
	CFLAGS += -I$(LUA_INCDIR)
endif

.PHONY: clean

all: $(TARGET)

debug: CFLAGS += -ggdb -UNDEBUG
debug: all

$(TARGET): $(OBJECTS)
	$(CC) -o $(TARGET) $^ $(CFLAGS) $(LIBS)

clean:
	rm -f $(OBJECTS) $(TARGET)
