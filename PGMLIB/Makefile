CC=g++ 
CFLAGS= -std=c++11  -g
INCLUDE=-I ./includes -I ./external/Linux/HDFS/include
OBJDIR=./Linux/obj/
LIBDIR=./Linux/lib/
LIBSRCDIR=./PGMLIB/
LIBSRC = $(notdir $(wildcard $(LIBSRCDIR)*.cpp))
LIBOBJ = $(LIBSRC:.cpp=.o)
LIBNAME = PGMLIB.a

#define default action
default:
	@echo "make factor project v1.0"
	@echo "use make all"
all: $(LIBNAME)


$(LIBNAME): $(LIBOBJ)
	ar rcs $(LIBDIR)$(LIBNAME) $(foreach n,$(LIBOBJ),$(OBJDIR)$(n))

$(LIBOBJ): %.o:$(LIBSRCDIR)%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -c $< -o $(OBJDIR)$@
clean: 
	-rm $(LIBDIR)*.* $(OBJDIR)*.*
