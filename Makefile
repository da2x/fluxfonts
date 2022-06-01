# Fluxfonts – a continual font generator for increased privacy
# Copyright 2012–2017, Daniel Aleksandersen
# All rights reserved.
#
# SPDX-License-Identifier: BSD-2-Clause-FreeBSD
# License-Filename: COPYING
#
# This file is part of Fluxfonts.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
# • Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
#
# • Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# “AS IS” AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

SHELL = /bin/sh

NAME  = Fluxfonts
LNAME = fluxfonts
PROGRAM = fluxfontd
REVERSE_DOMAIN = no.priv.daniel.$(LNAME)
SHORTDESC = $(NAME), a continual font generator for increased privacy.
LONGDDESC = The $(PROGRAM) daemon for $(SHORTDESC)

VERSION = 2.0
AUTHOR = Daniel Aleksandersen

OBJS = lib/buffer.o\
       lib/familyname.o\
       lib/opentype.o\
       lib/utils.o\
       lib/tables/cff.o\
       lib/tables/cmap.o\
       lib/tables/head.o\
       lib/tables/hhea.o\
       lib/tables/hmtx.o\
       lib/tables/maxp.o\
       lib/tables/name.o\
       lib/tables/os2.o\
       lib/tables/post.o\
       lib/tables/tables.o\
       main.o

DOCS = AUTHORS COPYING README

# directories
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
DOCDIR = $(PREFIX)/share/doc/$(LNAME)
TEMPDIR = /var/tmp
LAUNCHDDIR = /Library/LaunchDaemons
LAUNCHDCONF = $(LAUNCHDDIR)/$(LNAME).plist
SYSTEMDDIR = $(PREFIX)/lib/systemd/system
SYSTEMDCONF = $(SYSTEMDDIR)/$(LNAME).service
DISTROOTDIR = $(TEMPDIR)/$(LNAME).dist
DISTPKGDIR = $(DISTROOTDIR)/dest_root
DISTSCRIPTSDIR = $(DISTROOTDIR)/scripts
OSXPKGPOSTINSTALL = $(DISTSCRIPTSDIR)/postinstall

LDFLAGS += -lm

# OS differences
ifeq ($(shell sh -c 'uname -s'),Linux)
LDFLAGS += -lsystemd
CFLAGS += -DSYSTEMD
endif
ifeq ($(shell sh -c 'uname -s'),FreeBSD)
INSTALLFLAGS = -S
LDFLAGS += -liconv -L/usr/local/lib
CFLAGS += -I/usr/local/include
endif
ifeq ($(shell sh -c 'uname -s'),Darwin)
INSTALLFLAGS = -S
LDFLAGS += -liconv
PACKAGE_OSX = $(NAME)-$(VERSION).pkg
TEMPDIR = /private/var/tmp
endif

# programs and compiler
INSTALL_PROGRAM = /usr/bin/install
LAUNCHDUTIL = /bin/launchctl
DEFAULTSUTIL = /usr/bin/defaults
SERVICEUTIL = /usr/sbin/service
INITDEFUTIL = /sbin/insserv
OSXPKGBUILD = /usr/bin/pkgbuild
CC ?= clang
CFLAGS += -Wall -std=c99

# launchd or systemd based on available directories
ifneq ($(wildcard $(SYSTEMDDIR)),)
CONFIG_SYSTEMD = install-systemd
DECONFIG_SYSTEMD = deconfigure-systemd
endif

ifneq ($(wildcard $(LAUNCHDDIR)),)
CONFIG_LAUNCHD = install-launchd
DECONFIG_LAUNCHD = deconfigure-launchd
endif


# sources for daemon configuration files
define newline


endef

define LAUNCHDCONF-SRC
$(DEFAULTSUTIL) write $(DISTPKGDIR)$(LAUNCHDCONF) RunAtLoad -boolean TRUE
$(DEFAULTSUTIL) write $(DISTPKGDIR)$(LAUNCHDCONF) Label -string $(REVERSE_DOMAIN)
$(DEFAULTSUTIL) write $(DISTPKGDIR)$(LAUNCHDCONF) ProgramArguments -array-add $(BINDIR)/$(PROGRAM)

endef

define SYSTEMDCONF-SRC
[Unit]
Description=$(SHORTDESC)

[Service]
Type=notify
ExecStart=$(BINDIR)/$(PROGRAM)
PIDFile=/run/$(NAME).pid
Nice=15
PrivateDevices=true
ProtectKernelTunables=true
ProtectControlGroups=true
ProtectSystem=strict
ReadWritePath=-/run/ -/usr/local/share/fonts/ +/var/lib/fluxfonts/

[Install]
WantedBy=multi-user.target

endef


all: build

build: $(PROGRAM)

lib/buffer.o: lib/buffer.c
	$(CC) $(CFLAGS) -o $@ -c lib/buffer.c

lib/familyname.o: lib/familyname.c lib/buffer.h lib/define.h lib/utils.h
	$(CC) $(CFLAGS) -o $@ -c lib/familyname.c

lib/opentype.o: lib/opentype.c lib/buffer.h lib/define.h lib/familyname.h lib/tables/*.h
	$(CC) $(CFLAGS) -o $@ -c lib/opentype.c

lib/utils.o: lib/utils.c lib/define.h
	$(CC) $(CFLAGS) -o $@ -c lib/utils.c

lib/tables/cff.o: lib/tables/cff.c lib/buffer.h lib/familyname.h
	$(CC) $(CFLAGS) -o $@ -c lib/tables/cff.c

lib/tables/cmap.o: lib/tables/cmap.c lib/buffer.h
	$(CC) $(CFLAGS) -o $@ -c lib/tables/cmap.c

lib/tables/head.o: lib/tables/head.c lib/buffer.h
	$(CC) $(CFLAGS) -o $@ -c lib/tables/head.c

lib/tables/hhea.o: lib/tables/hhea.c lib/buffer.h
	$(CC) $(CFLAGS) -o $@ -c lib/tables/hhea.c

lib/tables/hmtx.o: lib/tables/hmtx.c lib/buffer.h
	$(CC) $(CFLAGS) -o $@ -c lib/tables/hmtx.c

lib/tables/maxp.o: lib/tables/maxp.c lib/buffer.h
	$(CC) $(CFLAGS) -o $@ -c lib/tables/maxp.c

lib/tables/name.o: lib/tables/name.c lib/buffer.h lib/familyname.h
	$(CC) $(CFLAGS) -o $@ -c lib/tables/name.c

lib/tables/os2.o: lib/tables/os2.c lib/buffer.h lib/familyname.h
	$(CC) $(CFLAGS) -o $@ -c lib/tables/os2.c

lib/tables/post.o: lib/tables/post.c lib/buffer.h
	$(CC) $(CFLAGS) -o $@ -c lib/tables/post.c

lib/tables/tables.o: lib/tables/tables.c lib/buffer.h
	$(CC) $(CFLAGS) -o $@ -c lib/tables/tables.c

main.o: main.c lib/define.h lib/buffer.h lib/familyname.h lib/utils.h lib/opentype.h
	$(CC) $(CFLAGS) -o $@ -c main.c

$(PROGRAM): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(PROGRAM) $(LDFLAGS)

install: build install-docs
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(BINDIR)
	$(INSTALL_PROGRAM) $(INSTALLFLAGS) $(PROGRAM) $(DESTDIR)$(BINDIR)

install-docs:
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(DOCDIR)
	$(INSTALL_PROGRAM) $(INSTALLFLAGS) $(DOCS) $(DESTDIR)$(DOCDIR)

install-daemon: $(CONFIG_LAUNCHD) $(CONFIG_SYSTEMD)

install-dist-osx: $(NAME)-$(VERSION).pkg
	/usr/sbin/installer --pkg $(NAME)-$(VERSION).pkg --PROGRAM /

uninstall: $(DECONFIG_LAUNCHD) $(DECONFIG_SYSTEMD)
	rm $(BINDIR)/$(PROGRAM)
	rm -rf $(DOCDIR)

configure-launchd:
	-rm $(DISTPKGDIR)$(LAUNCHDCONF)
	$(INSTALL_PROGRAM) -d $(DISTPKGDIR)$(LAUNCHDDIR)
	$(LAUNCHDCONF-SRC)

install-launchd: configure-launchd
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(LAUNCHDDIR)
	$(INSTALL_PROGRAM) $(DISTPKGDIR)$(LAUNCHDCONF) $(DESTDIR)$(LAUNCHDDIR)
	$(LAUNCHDUTIL) load $(DESTDIR)$(LAUNCHDCONF)

deconfigure-launchd:
	-$(LAUNCHDUTIL) unload $(LAUNCHDCONF)
	-rm $(LAUNCHDCONF)

configure-systemd:
	-rm $(DISTPKGDIR)$(SYSTEMDCONF)
	$(INSTALL_PROGRAM) -d $(DISTPKGDIR)$(SYSTEMDDIR)
	printf '$(subst $(newline),\n,${SYSTEMDCONF-SRC})' > $(DISTPKGDIR)$(SYSTEMDCONF)

install-systemd: configure-systemd
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(SYSTEMDDIR)
	$(INSTALL_PROGRAM) $(DISTPKGDIR)$(SYSTEMDCONF) $(DESTDIR)$(SYSTEMDDIR)

deconfigure-systemd:
	-rm $(SYSTEMDCONF)

dist: $(PACKAGE_OSX)

$(NAME)-$(VERSION).pkg: build configure-launchd
	-rm $(NAME)-$(VERSION).pkg
	$(INSTALL_PROGRAM) -d $(DISTPKGDIR)$(BINDIR)
	$(INSTALL_PROGRAM) -d $(DISTPKGDIR)$(LAUNCHDDIR)
	$(INSTALL_PROGRAM) -d $(DISTSCRIPTSDIR)
	$(INSTALL_PROGRAM) -d $(DISTPKGDIR)$(DOCDIR)
	$(INSTALL_PROGRAM) $(INSTALLFLAGS) $(PROGRAM) $(DISTPKGDIR)$(BINDIR)
	$(INSTALL_PROGRAM) $(INSTALLFLAGS) $(DOCS) $(DISTPKGDIR)$(DOCDIR)
	echo "#!$(SHELL)\n$(LAUNCHDUTIL) load $(LAUNCHDCONF)\nexit 0" > $(OSXPKGPOSTINSTALL)
	chmod +x $(OSXPKGPOSTINSTALL)
	$(OSXPKGBUILD) --identifier $(REVERSE_DOMAIN) --version $(VERSION) --root $(DISTPKGDIR) --ownership recommended --scripts $(DISTSCRIPTSDIR) $(NAME)-$(VERSION).pkg

$(LNAME)-$(VERSION).tar.gz: build maintainer-clean
	mkdir $(TEMPDIR)/$(LNAME)-$(VERSION)
	cp -r . $(TEMPDIR)/$(LNAME)-$(VERSION)
	-rm -rf $(TEMPDIR)/$(LNAME)-$(VERSION)/.*
	mv $(TEMPDIR)/$(LNAME)-$(VERSION) ./
	tar -c -z -f $(LNAME)-$(VERSION).tar.gz $(LNAME)-$(VERSION)

clean:
	-rm $(OBJS)
	-rm -rf $(PROGRAM).dSYM

distclean:
	-rm -rf $(DISTROOTDIR) $(TEMPDIR)/$(NAME)-$(VERSION) $(LNAME)-$(VERSION)/ $(NAME)-$(VERSION).pkg $(LNAME)-$(VERSION).tar.gz

maintainer-clean: clean distclean
	-rm $(PROGRAM)

