# This is the name of your executable
EXE = bm_suite

#  Here is the name of all object files corresponding to the source
# code that you wrote in order to define the problem statement
OBJS = \
	./main_example.o\
	./bm_data.o\
	./cl_parameters.o\
	./bm_reader.o\
	./bm_rotation.o\
	./bm_validator.o\


# C++ Compiler command
CXX = g++

# C++ Compiler options
CXXFLAGS =  -O3 -fomit-frame-pointer -pipe -DNDEBUG  -Wcast-qual -Wall -Wpointer-arith -Wwrite-strings -Wconversion -Wno-unknown-pragmas -g

# additional C++ Compiler options for linking
#CXXLINKFLAGS =  -Wl


#libs
LIBS = -lpthread -lm -Wl


#includes
INCL_DIR = ../include/
INCLUDES = 	$(INCL_DIR)
#CPLEX_H = $(ENV_CPLEX_H)
USER_INCDIRS = include $(INCLUDES)
CYGPATH_W = echo

INCL = `for i in $(USER_INCDIRS); do $(CYGPATH_W) $$i | sed -e 's/^/-I/'; done` 



all: $(EXE)

.SUFFIXES: .cpp .c .o .obj

$(EXE): $(OBJS)
	bla=;\
	for file in $(OBJS); do bla="$$bla `$(CYGPATH_W) $$file`"; done; \
	$(CXX) $(CXXLINKFLAGS) $(CXXFLAGS) -o $@ $$bla $(LIBS)

clean:
	rm -rf $(EXE) $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) $(INCL) -c -o $@ `test -f '$<' || echo '$(SRCDIR)/'`$<


.cpp.obj:
	$(CXX) $(CXXFLAGS) $(INCL) -c -o $@ `if test -f '$<'; then $(CYGPATH_W) '$<'; else $(CYGPATH_W) '$(SRCDIR)/$<'; fi`

