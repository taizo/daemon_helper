CFLAGS = -DCONFIG_FILE="\"${sysconfdir}/helper.conf\""  -DLOG_FILE="\"/var/log/${PACKAGE}-helper.log\""  -DLOCK_FILE="\"/var/lock/subsys/${PACKAGE}-helper\""
sbin_PROGRAMS = helper
helper_CFLAGS = $(CFLAGS) $(AM_CFLAGS)
helper_LDFLAGS =

helper_SOURCES = helper.c helper.h
helper_SUBDIRS = sample

AM_CXXFLAGS=-Wall -include $(CONFIG_HEADER)
AM_CFLAGS=-Wall -include $(CONFIG_HEADER)
