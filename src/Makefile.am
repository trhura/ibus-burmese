# vim:set noet ts=4:
#
# ibus-anthy - The Anthy engine for IBus
#
# Copyright (c) 2007-2008 Huang Peng <shawn.p.huang@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.


libexec_PROGRAMS = ibus-engine-burmese

ibus_engine_burmese_SOURCES = \
	main.c \
	engine.c \
	engine.h \
	debug.c	\
	debug.h
	$(NULL)
ibus_engine_burmese_CFLAGS = \
	@IBUS_CFLAGS@ \
	-DPKGDATADIR=\"$(pkgdatadir)\"
	$(NULL)
ibus_engine_burmese_LDFLAGS = \
	@IBUS_LIBS@ \
	$(NULL)

component_DATA = \
	burmese.xml \
	$(NULL)
componentdir = @datadir@/ibus/component

EXTRA_DIST = \
	burmese.xml.in \
	$(NULL)

CLEANFILES = \
	burmese.xml \
	$(NULL)

burmese.xml: burmese.xml.in
	$(AM_V_GEN) \
	( \
		libexecdir=${libexecdir}; \
		pkgdatadir=${pkgdatadir}; \
		s=`cat $<`; \
		eval "echo \"$${s}\""; \
	) > $@

test: ibus-engine-burmese
	$(builddir)/ibus-engine-burmese
