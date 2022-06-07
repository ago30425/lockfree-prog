# Cross compile
CC = gcc
AR = ar

# Flags
CFLAGS = -Wall -Wextra -Werror -O2 -fPIC
DEPFLAGS = -MMD -MP -MF $(DEPDIR)/$*.d -MT $@

# Path
SRCS = $(wildcard *.c)
OBJS = $(patsubst %.c, %.o, $(SRCS))
DEPDIR = .deps
DEPFILES = $(patsubst %.c,$(DEPDIR)/%.d,$(wildcard *.c))

