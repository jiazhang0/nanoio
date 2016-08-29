include $(TOPDIR)/env.mk
include $(TOPDIR)/rules.mk

LIB_NAME := libnanoio

LIB_TARGETS := $(LIB_NAME).so $(LIB_NAME).a

OBJS_$(LIB_NAME) = \
		   nnio_build.o \
		   nnio.o \
		   options.o \
		   socket.o \
		   endpoint.o \
		   util.o

CFLAGS += -fpic

all: $(LIB_TARGETS) Makefile

clean:
	@$(RM) $(LIB_TARGETS) $(OBJS_$(LIB_NAME)) $(addsuffix .*, $(LIB_TARGETS)) nnio_build.c

install: all
	$(INSTALL) -d -m 755 $(DESTDIR)$(libdir)
	$(INSTALL) -m 755 $(LIB_NAME).a $(DESTDIR)$(libdir)
	$(INSTALL) -m 755 $(LIB_NAME).so $(DESTDIR)$(libdir)/$(LIB_NAME).so.$(NANOIO_VERSION)
	$(foreach x, $(LIB_NAME).so, ln -sfn $(x).$(NANOIO_VERSION) $(DESTDIR)$(libdir)/$(patsubst %,%.$(NANOIO_MAJOR_VERSION).$(NANOIO_MINOR_VERSION),$(x)); \
		ln -sfn $(x).$(NANOIO_MAJOR_VERSION).$(NANOIO_MINOR_VERSION) $(DESTDIR)$(libdir)/$(patsubst %,%.$(NANOIO_MAJOR_VERSION),$(x));)

$(LIB_NAME).so: $(OBJS_$(LIB_NAME))
	$(CC) $^ -o $@ $(CFLAGS) -shared -Wl,-soname,$(patsubst %.$(NANOIO_VERSION),%,$@)

$(LIB_NAME).a: $(OBJS_$(LIB_NAME))
	$(AR) rcs $@ $^

nnio_build.c: nnio_build.c.in
	sed -e "s~@@NANOIO_GIT_COMMIT@@~$(shell if [ -d $(TOPDIR)/.git ]; then git log -1 --pretty=format:%H | tr -d '\n'; elif [ -f $(TOPDIR)/commit ]; then cat $(TOPDIR)/commit | tr -d '\n'; else echo -n ???????; fi)~" \
		-e "s~@@NANOIO_BUILD_MACHINE@@~$(shell whoami | tr -d '\n'; echo -n @; uname -a | tr -d '\n' | awk '{gsub("~","\\~"); print $0 }')~" < $^ > $@
