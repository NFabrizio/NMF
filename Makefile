PROGRAM	= nmf nmfOpt nmfPar nmfOptPar nmfVec
CC	= gcc
CFLAGS	= -O3 -g
CPARFLAGS = -O3 -g -fopenmp
SRCS	= timer.c nmf.c feature.c testMatricies.c
ALG_SRCS = MT.h learn.h learn.c learnOpt.c learnPar.c learnOptPar.c learnVec.c
OBJS	= $(SRCS:.c=.o)
HEADERS	= $(SRCS:.c=.h)
LDFLAGS	= -lm
VERSION	= 0.1
PKGNAME	= nmf-$(VERSION)
DIRS	= include/ test_data/
DISTFILES = README.md generateData.sh testPython.sh testAll.sh test.sh pythonNMF.py report.pdf Makefile

all: depend $(PROGRAM)

nmf: learn.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LDFLAGS)
nmfOpt: learnOpt.c $(OBJS)
	$(CC) $(CFLAGS) -o $@ $< $(OBJS) $(LDFLAGS)
nmfPar: learnPar.c $(OBJS)
	$(CC) $(CPARFLAGS) -o $@ $< $(OBJS) $(LDFLAGS)
nmfOptPar: learnOptPar.c $(OBJS)
	$(CC) $(CPARFLAGS) -o $@ $< $(OBJS) $(LDFLAGS)
nmfVec: learnVec.c $(OBJS)
	$(CC) $(CPARFLAGS) -o $@ $< $(OBJS) $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) -c $<

depend:
	@$(CC) -MM $(SRCS) > .depend
clean:
	@rm -f .depend $(OBJS)

tar: $(SRCS) $(ALG_SRCS) $(HEADERS) $(wildcard $(addsuffix *, $(DIRS))) $(DISTFILES)
	tar -czvf finalProject.tar $^

-include .depend
