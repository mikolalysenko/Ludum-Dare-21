# Makefile for C++ project with auto-dependencies and multiple build options
# Copyright (C) 2000-2003 Marc Mongenet

###############################################################################
# USER DEFINED VARIABLES
# The following variable values must be suited to the build environment.
###############################################################################

# name of the file to build
EXE = a.out

# C++ compiler
CXX = g++ -std=c++0x

# C compiler
CC = gcc

# source files directory
srcdir = src

# build directory
builddir = obj

# preprocessor options to find all included files
INC_PATH = -I$(srcdir) -I/usr/local/include -Iincludes

# libraries link options ('-lm' is common to link with the math library)
LNK_LIBS = -L/usr/local/lib -lglfw -lGL -lGLU -lrt -pthread -ldl -lm -lc `sdl-config --cflags --libs`

# other compilation options
COMPILE_OPTS = -pthread

# basic compiler warning options (for GOAL_EXE)
BWARN_OPTS = 

# extented compiler warning options (for GOAL_DEBUG)
EWARN_OPTS = $(BWARN_OPTS) -pedantic\
	     -Wtraditional -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align\
	     -Wwrite-strings -Wstrict-prototypes -Wredundant-decls\
	     -Wnested-externs -Woverloaded-virtual -Winline

###############################################################################
# INTERNAL VARIABLES
# The following variable values should not depend on the build environment.
# You may safely stop the configuration at this line.
###############################################################################

# You may freely change the following goal names if you dislike them.
GOAL_DEBUG = debug
GOAL_PROF = prof
GOAL_EXE = all

# build options for GOAL_DEBUG (executable for debugging) goal
ifeq "$(MAKECMDGOALS)" "$(GOAL_DEBUG)"

 # specific options for debugging
 GOAL_OPTS = -g
 # compilation verification options
 WARN_OPTS = $(BWARN_OPTS)
 # optimization options
 OPTIMISE_OPTS =
 # dependencies must be up to date

else

 # build options for GOAL_PROF (executable for profiling) goal
 ifeq "$(MAKECMDGOALS)" "$(GOAL_PROF)"

  # specific options for profiling
  GOAL_OPTS = -pg
  # compilation verification options
  WARN_OPTS = $(BWARN_OPTS)
  # optimization options
  OPTIMISE_OPTS = -O2
  # dependencies must be up to date

 else

  # build options for GOAL_EXE (optimized executable) goal
  ifeq "$(MAKECMDGOALS)" "$(GOAL_EXE)"

   # specific options for optimized executable
   GOAL_OPTS = -s
   # compilation verification options
   WARN_OPTS = $(BWARN_OPTS)
   # optimization options
   OPTIMISE_OPTS = -O3 -fomit-frame-pointer
   # dependencies must be up to date

  else

  endif
 endif
endif

# preprocessor options
CPPOPTS = $(INC_PATH)

# compiler options
CXXOPTS = $(GOAL_OPTS) $(COMPILE_OPTS) $(WARN_OPTS) $(OPTIMISE_OPTS)

# linker options
LDOPTS = $(GOAL_OPTS) $(LNK_LIBS)

# source files in this project
cppsources := $(wildcard $(srcdir)/*.cc)
csources := $(wildcard $(srcdir)/*.c)
sources := $(cppsources) $(csources)

# object files in this project
cppobjs := $(notdir $(cppsources))
cppobjs := $(addprefix $(builddir)/, $(cppobjs))
cobjs := $(notdir $(csources))
cobjs := $(addprefix $(builddir)/, $(cobjs))
objs := $(cppobjs:.cc=.o) $(cobjs:.c=.o)

# executable with full path
exe = $(EXE)

###############################################################################
# TARGETS
###############################################################################

# Delete the target file of a rule if the command used to update it failed.
# Do that because the newly generated target file may be corrupted but appear
# up to date.
.DELETE_ON_ERROR:

# Clear default suffix list to disable all implicit rules.
.SUFFIXES:

# usage message for this makefile
.PHONY:	usage
usage:
	@echo "GOAL	EFFECT"
	@echo "----	------"
	@echo "usage	print this message"
	@echo "list	list the source files"
	@echo "test	Does a test run of the executable"
	@echo "$(GOAL_EXE)	build the executable"
	@echo "$(GOAL_DEBUG)	build the executable with debug options"
	@echo "$(GOAL_PROF)	build the executable with profiling options"
	@echo "clean	remove all built files"

# If source files exist then build the EXE file.
.PHONY:	$(GOAL_EXE)
ifneq "$(strip $(sources))" ""
$(GOAL_EXE):	$(exe)
else
$(GOAL_EXE):
	@echo "No source file found."
endif

# GOAL_DEBUG and GOAL_PROF targets use the same rules than GOAL_EXE.
.PHONY:	$(GOAL_DEBUG)
$(GOAL_DEBUG):	$(GOAL_EXE)

.PHONY:	$(GOAL_PROF)
$(GOAL_PROF):	$(GOAL_EXE)

###############################################################################
# BUILDING
# Note: CPPFLAGS, CXXFLAGS or LDFLAGS are not used but may be specified by the
# user at make invocation.
###############################################################################

valgrind: $(exe) $(datadir)
	valgrind --log-file=valgrind.log ./$(exe)

test: $(exe) $(datadir)
	./$(exe)

$(datadir):
	mkdir $(datadir)

$(builddir):
	mkdir $(builddir)

# linking
$(exe):	$(objs)
	$(CXX) $^ -o $@ $(LDOPTS) $(LDFLAGS)

# explicit definition of the implicit rule used to compile source files
$(builddir)/%.o:	src/%.cc
	$(CXX) -c $< $(CPPOPTS) $(CXXOPTS) $(CPPFLAGS) $(CXXFLAGS) -o $@

$(builddir)/%.o:	src/%.c
	$(CC) -c $< $(CPPOPTS) $(CXXOPTS) $(CPPFLAGS) $(CXXFLAGS) -o $@


###############################################################################
# NON-BUILD TARGETS
###############################################################################

# List the source files
.PHONY:	list
list:
	@echo $(sources) | tr [:space:] \\n

# Remove all files that are normally created by building the program.
.PHONY:	clean
clean:
	rm -f $(exe) $(objs)
