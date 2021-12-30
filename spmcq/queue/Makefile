SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))

INCS = -Iinclude -Iinclude/internal

TARGET = libqueue.a

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(AR) rcs $(TARGET) $^

%.o: %.c
	$(CC) $(CFLAGS) $(INCS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
