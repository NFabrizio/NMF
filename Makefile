PROGRAM	= nmf nmfOpt nmfPar nmfOptPar
CC	= gcc
CFLAGS	= -O3 -g
CPARFLAGS = -O3 -g -fopenmp
SRCS	= timer.c nmf.c feature.c testMatricies.c
OBJS	= $(SRCS:.c=.o)
HEADERS	= $(SRCS:.c=.h)
LDFLAGS	= -lm
VERSION	= 0.1
PKGNAME	= nmf-$(VERSION)
DISTDIR	= ../dist
DISTFILES	= $(SRCS) $(HEADERS) Makefile

all: depend $(PROGRAM)

nmf: learn.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LDFLAGS)
nmfOpt: learnOpt.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LDFLAGS)
nmfPar: learnPar.c $(OBJS)
	$(CC) $(CPARFLAGS) -o $@ $< $(OBJS) $(LDFLAGS)
nmfOptPar: learnOptPar.c $(OBJS)
	$(CC) $(CPARFLAGS) -o $@ $< $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

depend:
	@$(CC) -MM $(SRCS) > .depend
clean:
	@rm -f .depend $(OBJS)
pkg:
	@[ -d $(PKGNAME) ] || mkdir $(PKGNAME)
	@cp -p $(DISTFILES) $(PKGNAME)
	@tar czvf $(DISTDIR)/$(PKGNAME).tar.gz $(PKGNAME)
	@rm -r $(PKGNAME)

-include .depend
