CROSS_COMPILE ?=
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar
INSTALL ?= install

EXTRA_CFLAGS ?=
EXTRA_LDFLAGS ?=

DEBUG_BUILD ?=
DESTDIR ?=
prefix ?= /usr/local
libdir ?= $(prefix)/lib
bindir ?= $(prefix)/bin
sbindir ?= $(prefix)/sbin
includedir ?= $(prefix)/include

nanomsg_includedir ?= $(includedir)
nanomsg_libdir ?= $(libdir)

LDFLAGS := --warn-common --no-undefined --fatal-warnings \
	   $(patsubst $(join -Wl,,)%,%,$(EXTRA_LDFLAGS))
CFLAGS := -D_GNU_SOURCE -std=gnu11 -O2 -Wall -Werror \
	  $(addprefix -I, $(TOPDIR)/src/include $(nanomsg_includedir)) \
	  -L$(nanomsg_libdir) -lnanomsg -lpthread -lrt \
	  $(EXTRA_CFLAGS) $(addprefix $(join -Wl,,),$(LDFLAGS))

ifneq ($(DEBUG_BUILD),)
	CFLAGS += -ggdb -DDEBUG
endif
