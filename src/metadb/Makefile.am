#
#    Ophidia IO Server
#    Copyright (C) 2014-2016 CMCC Foundation
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

if DEBUG 
OPT+=-DDEBUG
endif

noinst_LTLIBRARIES=liboph_metadb.la

bin_PROGRAMS= oph_metadb_reader
if DEBUG
bin_PROGRAMS+=oph_metadb_client 
endif
bindir=${prefix}/bin

liboph_metadb_la_SOURCES = oph_metadb_interface.c oph_metadb_auxiliary.c
liboph_metadb_la_CFLAGS = $(OPT) -I. -I.. -I../common -I../iostorage -fPIC -DOPH_IO_SERVER_PREFIX=\"${prefix}\"
liboph_metadb_la_LIBADD= -L../common -ldebug -L../iostorage -loph_iostorage_data
liboph_metadb_la_LDFLAGS = -module -static

if DEBUG
oph_metadb_client_SOURCES = oph_metadb_client.c
oph_metadb_client_CFLAGS = $(OPT) -I../common -I../iostorage -I. -fPIC @INCLTDL@  -DOPH_IO_SERVER_PREFIX=\"${prefix}\"
oph_metadb_client_LDADD = -L.  -L../common -ldebug -lpthread -loph_metadb -loph_server_conf
oph_metadb_client_LDFLAGS= -Wl,-R -Wl,. 
endif

oph_metadb_reader_SOURCES = oph_metadb_reader.c
oph_metadb_reader_CFLAGS = $(OPT) -I../common -I../iostorage -I. -fPIC @INCLTDL@  -DOPH_IO_SERVER_PREFIX=\"${prefix}\"
oph_metadb_reader_LDADD = -L. -L../common -ldebug -loph_metadb -loph_server_conf
oph_metadb_reader_LDFLAGS= -Wl,-R -Wl,. 
