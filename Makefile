CC=		gcc
CXX=		g++
CFLAGS=		-g -Wall -O2 # -Wno-unused-but-set-variable
CXXFLAGS=	$(CFLAGS)
DFLAGS=		-DHAVE_PTHREAD # -D_FILE_OFFSET_BITS=64
OBJS=		utils.o bwt.o bwtio.o bwtaln.o bwtgap.o is.o \
			bntseq.o bwtmisc.o bwtindex.o stdaln.o simple_dp.o \
			bwaseqio.o bwase.o bwape.o kstring.o cs2nt.o \
			bwape1.o bwape2.o bwape3.o bwape4.o bwapese1.o bwapeio1.o \
			bwase1.o bwase4.o bwaseio1.o \
			bwtsw2_core.o bwtsw2_main.o bwtsw2_aux.o bwt_lite.o \
			bwtsw2_chain.o bamlite.o
PROG=		bwa
INCLUDES=	
LIBS=		-lm -lz -lpthread -Lbwt_gen -lbwtgen
SUBDIRS=	. bwt_gen

.SUFFIXES:.c .o .cc

.c.o:
		$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) $< -o $@
.cc.o:
		$(CXX) -c $(CXXFLAGS) $(DFLAGS) $(INCLUDES) $< -o $@

all:$(PROG)

lib-recur all-recur clean-recur cleanlocal-recur install-recur:
		@target=`echo $@ | sed s/-recur//`; \
		wdir=`pwd`; \
		list='$(SUBDIRS)'; for subdir in $$list; do \
			cd $$subdir; \
			$(MAKE) CC="$(CC)" CXX="$(CXX)" DFLAGS="$(DFLAGS)" CFLAGS="$(CFLAGS)" \
				INCLUDES="$(INCLUDES)" $$target || exit 1; \
			cd $$wdir; \
		done;

lib:

bwt_gen/libbwtgen.a:
		make --directory=bwt_gen

bwa:bwt_gen/libbwtgen.a $(OBJS) main.c main.h
		d=`date`;\
		$(CC) $(CFLAGS) -DBLDDATE="$$d" -c main.c -o main.o ;\
		$(CC) $(CFLAGS) $(DFLAGS) $(OBJS) main.o -o $@ $(LIBS)

depend:
		makedepend $(DFLAGS) -Y *.c

cleanlocal:
		rm -f gmon.out *.o a.out $(PROG) *~ *.a bin/*

clean:cleanlocal-recur

# DO NOT DELETE

bamlite.o: bamlite.h utils.h
bntseq.o: bntseq.h kseq.h main.h utils.h
bwape.o: bntseq.h bwatpx.h bwt.h bwtaln.h khash.h ksort.h kstring.h kvec.h
bwape.o: stdaln.h utils.h
bwape1.o: bntseq.h bwatpx.h bwt.h bwtaln.h khash.h ksort.h kstring.h kvec.h
bwape1.o: stdaln.h
bwape2.o: bntseq.h bwatpx.h bwt.h bwtaln.h kstring.h kvec.h stdaln.h
bwape3.o: bntseq.h bwatpx.h bwt.h bwtaln.h kstring.h kvec.h stdaln.h
bwape4.o: bntseq.h bwatpx.h bwt.h bwtaln.h kstring.h kvec.h stdaln.h
bwapeio1.o: bntseq.h bwatpx.h bwt.h bwtaln.h kstring.h kvec.h stdaln.h
bwapeio1.o: utils.h
bwapese1.o: bntseq.h bwatpx.h bwt.h bwtaln.h kstring.h kvec.h stdaln.h
bwase.o: bntseq.h bwase.h bwatpx.h bwt.h bwtaln.h kstring.h kvec.h stdaln.h
bwase.o: utils.h
bwase1.o: bntseq.h bwatpx.h bwt.h bwtaln.h kstring.h kvec.h stdaln.h
bwase4.o: bntseq.h bwatpx.h bwt.h bwtaln.h kstring.h kvec.h stdaln.h
bwaseio1.o: bntseq.h bwatpx.h bwt.h bwtaln.h kstring.h kvec.h stdaln.h
bwaseio1.o: utils.h
bwaseqio.o: bamlite.h bwt.h bwtaln.h kseq.h stdaln.h utils.h
bwatpx.o: bntseq.h bwt.h bwtaln.h kstring.h kvec.h stdaln.h
bwt.o: bwt.h utils.h
bwt_lite.o: bwt_lite.h
bwtaln.o: bwt.h bwtaln.h bwtgap.h stdaln.h utils.h
bwtgap.o: bwt.h bwtaln.h bwtgap.h stdaln.h
bwtindex.o: bntseq.h bwt.h main.h utils.h
bwtio.o: bwt.h utils.h
bwtmisc.o: bntseq.h bwt.h main.h utils.h
bwtsw2.o: bntseq.h bwt.h bwt_lite.h
bwtsw2_aux.o: bntseq.h bwt.h bwt_lite.h bwtsw2.h kseq.h ksort.h kstring.h
bwtsw2_aux.o: stdaln.h utils.h
bwtsw2_chain.o: bntseq.h bwt.h bwt_lite.h bwtsw2.h ksort.h
bwtsw2_core.o: bntseq.h bwt.h bwt_lite.h bwtsw2.h khash.h ksort.h kvec.h
bwtsw2_main.o: bntseq.h bwt.h bwt_lite.h bwtsw2.h utils.h
cs2nt.o: bwt.h bwtaln.h stdaln.h
kstring.o: kstring.h
main.o: main.h utils.h
simple_dp.o: kseq.h stdaln.h utils.h
stdaln.o: stdaln.h
utils.o: utils.h
