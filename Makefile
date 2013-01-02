# Fluxfonts – a continual font generator for increased privacy
# Copyright 2012–2013, Daniel Aleksandersen
# All rights reserved.
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

VERSION = 1.0
AUTHOR = Daniel Aleksandersen

SRCS = main.c $(wildcard lib/*.c) $(wildcard lib/tables/*.c)
DOCS = AUTHORS COPYING README

# directories
PREFIX ?= /usr/local
BINDIR = $(PREFIX)/bin
DOCDIR = $(PREFIX)/share/doc/$(LNAME)
TEMPDIR = /var/tmp
LAUNCHDDIR = /Library/LaunchDaemons
LAUNCHDCONF = $(LAUNCHDDIR)/$(LNAME).plist
UPSTARTDIR = /etc/init
UPSTARTCONF = $(UPSTARTDIR)/$(LNAME).conf
SYSVINITDIR = /etc/init.d
SYSVINITCONF = $(SYSVINITDIR)/$(LNAME)
DISTROOTDIR = $(TEMPDIR)/$(LNAME).dist
DISTPKGDIR = $(DISTROOTDIR)/dest_root
DISTSCRIPTSDIR = $(DISTROOTDIR)/scripts
OSXPKGPOSTINSTALL = $(DISTSCRIPTSDIR)/postinstall

# OS differences from GNU/Linuses
ifeq ($(shell sh -c 'uname -s'),FreeBSD)
INSTALLFLAGS = -S
LDFLAGS += -liconv -L/usr/local/lib
CFLAGS += -I/usr/local/include
endif
ifeq ($(shell sh -c 'uname -s'),Darwin)
INSTALLFLAGS = -S
LDFLAGS += -liconv
ARCH = -arch x86_64 -arch i386
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
CFLAGS += -Wall -std=c99 -g

# launchd, upstart, or sysvinit based on available directories
ifneq ($(wildcard $(SYSVINITDIR)),)
CONFIG_SYSVINIT = install-sysvinit
DECONFIG_SYSVINIT = deconfigure-sysvinit
endif

ifneq ($(wildcard $(UPSTARTDIR)),)
CONFIG_UPSTART = install-upstart
DECONFIG_UPSTART = deconfigure-upstart
CONFIG_SYSVINIT =
DECONFIG_SYSVINIT =
endif

ifneq ($(wildcard $(LAUNCHDDIR)),)
CONFIG_LAUNCHD = install-launchd
DECONFIG_LAUNCHD = deconfigure-launchd
CONFIG_SYSVINIT =
DECONFIG_SYSVINIT =
endif


# sources for daemon configuration files
define newline


endef

define LAUNCHDCONF-SRC
$(DEFAULTSUTIL) write $(DISTPKGDIR)$(LAUNCHDCONF) RunAtLoad -boolean TRUE
$(DEFAULTSUTIL) write $(DISTPKGDIR)$(LAUNCHDCONF) Label -string $(REVERSE_DOMAIN)
$(DEFAULTSUTIL) write $(DISTPKGDIR)$(LAUNCHDCONF) ProgramArguments -array-add $(BINDIR)/$(PROGRAM)

endef

define UPSTARTCONF-SRC
description "$(SHORTDESC)"
author "$(AUTHOR)"

start on filesystem
stop on runlevel S

respawn
respawn limit 10 10

expect fork
exec $(BINDIR)/$(PROGRAM)

endef

# Due to layers of different substitution interpretors it is necessary to use "$$$ "
# for a single literal "$". "$$" is used inside a comment for the same result.
# Several code monkeys where hurt while researching this.
define SYSVINITCONF-SRC
#!/bin/sh
### BEGIN INIT INFO
# Provides: $(LNAME)
# Required-Start: $$local_fs $$remote_fs $$syslog
# Required-Stop: $$local_fs
# Default-Start: 2 3 4 5
# Default-Stop: 0 1 6
# Short-Description: $(SHORTDESC)
# Description: $(LONGDDESC)
### END INIT INFO
# Author: Daniel Aleksandersen

# Start the service $(NAME)
start() {
  echo "Starting $(LNAME) daemon."
  $(BINDIR)/$(PROGRAM) &
}
# Restart the service $(NAME)
stop() {
  echo "Stopping $(LNAME) daemon."
  pkill $(PROGRAM)
}
### main ###
case "$$$ 1" in
  start)
    start
    ;;
  stop)
    stop
    ;;
  status)
    status $(PROGRAM)
    ;;
  restart|reload|condrestart)
    stop
    start
    ;;
  *)
    echo $$$ "Usage: $$$ 0 {start|stop|restart|reload|status}"
    exit 1
    ;;
esac
exit 0

endef

all: build

build: $(SRCS)
	$(CC) $(LDFLAGS) $(CFLAGS) $(ARCH) $(SRCS) -o $(PROGRAM) -lm

install: build
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(BINDIR)
	$(INSTALL_PROGRAM) $(INSTALLFLAGS) $(PROGRAM) $(DESTDIR)$(BINDIR)

install-all: build install install-doc install-daemon

install-doc:
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(DOCDIR)
	$(INSTALL_PROGRAM) $(INSTALLFLAGS) $(DOCS) $(DESTDIR)$(DOCDIR)

install-daemon: $(CONFIG_LAUNCHD) $(CONFIG_UPSTART) $(CONFIG_SYSVINIT)

install-dist-osx: $(NAME)-$(VERSION).pkg
	/usr/sbin/installer --pkg $(NAME)-$(VERSION).pkg --PROGRAM /

uninstall: $(DECONFIG_LAUNCHD) $(DECONFIG_UPSTART) $(DECONFIG_SYSVINIT)
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

configure-upstart:
	-rm $(DISTPKGDIR)$(UPSTARTCONF)
	$(INSTALL_PROGRAM) -d $(DISTPKGDIR)$(UPSTARTDIR)
	printf '$(subst $(newline),\n,${UPSTARTCONF-SRC})' > $(DISTPKGDIR)$(UPSTARTCONF)

install-upstart: configure-upstart
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(UPSTARTDIR)
	$(INSTALL_PROGRAM) $(DISTPKGDIR)$(UPSTARTCONF) $(DESTDIR)$(UPSTARTDIR)
	-$(SERVICEUTIL) $(LNAME) start

deconfigure-upstart:
	-$(SERVICEUTIL) $(LNAME) stop
	-rm $(UPSTARTCONF)

configure-sysvinit:
	-rm $(DISTPKGDIR)$(SYSVINITCONF)
	$(INSTALL_PROGRAM) -d $(DISTPKGDIR)$(SYSVINITDIR)
	printf '$(subst $(newline),\n,${SYSVINITCONF-SRC})' > $(DISTPKGDIR)$(SYSVINITCONF)

install-sysvinit: configure-sysvinit
	$(INSTALL_PROGRAM) -d $(DESTDIR)$(SYSVINITDIR)
	$(INSTALL_PROGRAM) $(DISTPKGDIR)$(SYSVINITCONF) $(DESTDIR)$(SYSVINITDIR)
	-$(INITDEFUTIL) $(LNAME)
	-$(SERVICEUTIL) $(LNAME) start

deconfigure-sysvinit:
	-$(SERVICEUTIL) $(LNAME) stop
	-$(SYSVINITDIR) --remove $(LNAME)
	-rm $(SYSVINITCONF)

dist: $(PACKAGE_OSX)

$(NAME)-$(VERSION).pkg: all configure-launchd
	-rm $(NAME)-$(VERSION).pkg
	$(INSTALL_PROGRAM) -d $(DISTPKGDIR)$(BINDIR)
	$(INSTALL_PROGRAM) -d $(DISTPKGDIR)$(LAUNCHDDIR)
	$(INSTALL_PROGRAM) -d $(DISTSCRIPTSDIR)
	$(INSTALL_PROGRAM) -d $(DISTPKGDIR)$(DOCDIR)
	$(INSTALL_PROGRAM) $(INSTALLFLAGS) $(PROGRAM) $(DISTPKGDIR)$(BINDIR)
	$(INSTALL_PROGRAM) $(INSTALLFLAGS) $(DOCS) $(DISTPKGDIR)$(DOCDIR)
	echo "#!$(SHELL)\n$(LAUNCHDUTIL) load $(LAUNCHDCONF)" > $(OSXPKGPOSTINSTALL)
	$(OSXPKGBUILD) --identifier $(REVERSE_DOMAIN) --version $(VERSION) --root $(DISTPKGDIR) --ownership recommended --scripts $(DISTSCRIPTSDIR) $(NAME)-$(VERSION).pkg

$(LNAME)-$(VERSION).tar.gz: maintainer-clean
	mkdir $(TEMPDIR)/$(LNAME)-$(VERSION)
	cp -r . $(TEMPDIR)/$(LNAME)-$(VERSION)
	-rm -rf $(TEMPDIR)/$(LNAME)-$(VERSION)/.*
	mv $(TEMPDIR)/$(LNAME)-$(VERSION) ./
	tar czvf $(LNAME)-$(VERSION).tar.gz $(LNAME)-$(VERSION)

clean:
	rm -rf $(PROGRAM).dSYM

distclean:
	rm -rf $(DISTROOTDIR) $(TEMPDIR)/$(NAME)-$(VERSION) $(LNAME)-$(VERSION)/ $(NAME)-$(VERSION).pkg $(LNAME)-$(VERSION).tar.gz

maintainer-clean: clean distclean
	rm -rf $(PROGRAM) *~ */*~ */*/*~
