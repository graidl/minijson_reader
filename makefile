# default location of MHLIB 
ifndef MHLIB
  MHLIB=../../mhlib
endif

include $(MHLIB)/makefile.common
CCFLAGS += -L$(MHLIB) -I$(MHLIB)

EXE=jsontest

HEADERS=

SRCS=jsontest.C

OBJS=$(SRCS:.C=.o)

DEPS=$(SRCS:.C=.d)

$(EXE): $(OBJS) $(MHLIB)/libmh.a
	$(CPP) $(CCFLAGS) -o $(EXE) $(OBJS) -lmh

all: $(EXE)

clean:
	rm -f $(OBJS) $(DEPS) $(EXE)

sinclude $(DEPS)
