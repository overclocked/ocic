# -------------------------------------------------------------------------- #
#                                                                            #
#                                Introduction                                #
#                                                                            #
# Principles:                                                                #
#     * This Makefile is very specifically for c libraries and apps.         #
#     * This Makefile builds a single package.                               #
#     * A Package is scoped to a single directory build.                     #
#                                                                            #
# (1) Directory Structure:                                                   #
#                                                                            #
#     /dev-environment/                                                      #
#                     /bin                << Required package executables    #
#                     /lib                << Required package libraries      #
#                     /include/(package)  << Required package includes       #
#                     /proj/              << This Project                    #
#                          /src       << This project source files           #
#                          /build     << This project working build dir      #
#                          /bin       << This project executable output      #
#                          /lib       << This project library output         #
#                          /test      << Unit & Integration tests            #
#     /usr/local/                                                            #
#               /bin                  << System install of execuatable       #
#               /lib                  << System install of lib files         #
#               /include/(package)/   << System install of headers           #
#                                                                            #
#                                                                            #
# (2) Standard Usage                                                         #
#                                                                            #
#     make             << Compile an executable binary                       #
#     make env         << Set up dev enviroonment                            #
#     make alib        << Compile an archive library                         #
#     make slib        << Compile a shared object library                    #
#     make dlib        << Compile a macos dynamic library                    #
#     make test        << Build and execute tests                            #
#     make tq          << Test with no output unless fail.                   #
#     make clean       << Clear up dependencies                              #
#     make publish     << Publish artifacts into the dev environment.        #
#     make unpublish   << Remove artifacts from the dev encvironment         #
#     make install-bin << Publish executable to the system                   #
#     make install-lib << Install library files and headers to the system.   #
#     make uninstall   << Uninstall any bin, lib, or .h files from the sys.  #
#                                                                            #
# (3) Conventions                                                            #
#                                                                            #
#     * the directory this is built in is the package name.                  #
#     * the package name is also the name of the single executable that.     #
#       can be built                                                         #
#     * the package name is the core name of any library files built, e.g.:  #
#       lib<package>.so                                                      #
#     * the "main" function is in the file src/<package>.c                   #
#     * that file will not be included in any library files built.           #
#     * the "main" function to drive the unit tests will be in the.          #
#       file: test/t-<package>.c                                             #
#     * the test file can take a parameter "-q" that suppresses test         #
#       reporting                                                            #
#     * otherwise, the unit tests will be reported to standard out           #
#     * if any tests fail, the test app returns an error code, failing       #
#       the build                                                            #
#     * the dev environment conforms to the directory structure, above.      #
#     * when installing to the system, it also uses the directory structure. #
#     * Using the convention of the OC dev environment, all libraries needed #
#       for compile should be referenced in the companion file: req_lib.mk   #
#     * TODO: mechanism for specifying and pulling companion libraries with  #
#       version.                                                             #
#     * Custom modifications to this file or an additional included file can #
#       be used for third party libraries that don't follow this convention: #
#       note the $(LINK_DIRS) and $(THIRD_PARTY_LIBS) variables.             #
#                                                                            #
#                                                                            #
# (4) Capabilities                                                           #
#                                                                            #
#     * standard C code                                                      #
#     * TODO: support for flex & bison                                       #
#                                                                            #
# -------------------------------------------------------------------------- #


# -------------------------------------------------------------------------- #
# (1) PLATFORM SPECIFIC                                                      #
# -------------------------------------------------------------------------- #

APPLE := APPLE
LINUX := linux

PLATFORM := $(strip                                                      \
	 $(findstring $(APPLE),                                              \
	     $(shell cpp -dM < /dev/null | awk '/__APPLE__/ {print $$2}' ) ) \
	 $(findstring $(LINUX),                                              \
	     $(shell cpp -dM < /dev/null | awk '/__LINUX__/ {print $$2}' ) ) )

# -------------------------------------------------------------------------- #
# (2) Compilation Tools                                                      #
# -------------------------------------------------------------------------- #

SHELL := /bin/bash
CC    := gcc
AR    := ar
LEX   := flex
YACC  := bison

# -------------------------------------------------------------------------- #
# (3) Model the directory structure                                          #
# -------------------------------------------------------------------------- #

PKG      := $(shell basename `pwd`)
TMP_SELF := $(shell pwd)
ENV_ROOT := $(TMP_SELF:%/$(PKG)=%)

DEV_INC_ROOT := $(ENV_ROOT)/include
DEV_INC_DIR  := $(DEV_INC_ROOT)/$(PKG)
DEV_LIB_DIR  := $(ENV_ROOT)/lib
DEV_BIN_DIR  := $(ENV_ROOT)/bin
OBJ_DIR  := $(ENV_ROOT)/$(PKG)/obj
BIN_DIR  := $(ENV_ROOT)/$(PKG)/bin
SRC_DIR  := $(ENV_ROOT)/$(PKG)/src
TEST_DIR := $(ENV_ROOT)/$(PKG)/test
TEST_APP := $(TEST_DIR)/t-$(PKG)
TEST_BIN_SRC := $(TEST_APP).c

BIN_APP  := $(BIN_DIR)/$(PKG)
BIN_SRC  := $(SRC_DIR)/$(PKG).c

AR_FILE  := lib$(PKG).a
SO_FILE  := lib$(PKG).so
DYN_FILE := lib$(PKG).dylib

AR_TARG  := $(BIN_DIR)/$(AR_FILE)
SO_TARG  := $(BIN_DIR)/$(SO_FILE)
DYN_TARG := $(BIN_DIR)/$(DYN_FILE)

SYS_ROOT    := /usr/local
SYS_BIN_DIR := $(SYS_ROOT)/bin
SYS_LIB_DIR := $(SYS_ROOT)/lib
SYS_INC_DIR := $(SYS_ROOT)/include/$(PKG)/

# -------------------------------------------------------------------------- #
# (4) Compute Files                                                          #
# -------------------------------------------------------------------------- #

SRC_FILES    := $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES    := $(filter-out $(OBJ_DIR)/$(PKG).o, \
                $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES)))
TEST_SOURCES := $(wildcard $(TEST_DIR)/*.c)
TEST_OBJECTS := $(filter-out $(TEST_DIR)/t-$(PKG).o, \
                $(patsubst $(TEST_DIR)/%.c, $(TEST_DIR)/%.o, $(TEST_SOURCES)))

# -------------------------------------------------------------------------- #
# (5) Required Libraries                                                     #
# -------------------------------------------------------------------------- #

include req_lib.mk

# -------------------------------------------------------------------------- #
# (5) Configure the compile                                                  #
# -------------------------------------------------------------------------- #

C_STD      := c99

C_FLAGS    := $(strip -fPIC -std=$(C_STD) -funsigned-char \
                        -Wbad-function-cast -Wmissing-prototypes )

AR_FLAGS   := -rcs
DYN_FLAGS  := -dynamiclib -undefined suppress -flat_namespace

ifeq ($(PLATFORM),$(APPLE))
    LIB_FLAGS   := -shared -undefined suppress -flat_namespace
    LINK_FLAGS  := -flat_namespace
else
    LIB_FLAGS   := -shared
    LINK_FLAGS  :=
endif

COMP_FLAGS := -W -Wall -Wshadow -Wpointer-arith -Wcast-qual $(ADDTL_FLAGS)
INCLUDES   := -I$(SRC_DIR) -I$(DEV_INC_ROOT)

LINK_LINE  := $(LINK_FLAGS) $(DEV_ENV_LIBS) -L$(DEV_LIB_DIR) $(LINK_DIRS) \
              $(THIRD_PARTY_LIBS) $(STD_LIBS)

# Set the environment variable DEBUG to true to build with debug flags.
ifdef DEBUG
  DBG_FLAGS := -ggdb
  D_FLAGS := $(D_FLAGS) -D DEBUG
else
  COMP_FLAGS := $(COMP_FLAGS) -O2
  D_FLAGS := $(D_FLAGS) -D NDEBUG
endif

CC_EXEC   := $(CC) $(COMP_FLAGS) $(DBG_FLAGS) $(C_FLAGS)

# -------------------------------------------------------------------------- #
# (6) System Build Rules                                                     #
# -------------------------------------------------------------------------- #

.PHONY: env clean alib slib dlib app test tq publish unpublish install-bin install-lib ocic

ocic: env alib publish
	@echo "Lookin good."

app: $(BIN_APP)

alib: $(AR_TARG)

slib: $(SO_TARG)

dlib: $(DYN_TARG)

env:
	@echo "Checking Environment..."
	@if [ ! -d $(DEV_INC_DIR) ]; then mkdir -p $(DEV_INC_DIR); fi;
	@if [ ! -d $(DEV_LIB_DIR) ]; then mkdir -p $(DEV_LIB_DIR); fi;
	@if [ ! -d $(DEV_BIN_DIR) ]; then mkdir -p $(DEV_BIN_DIR); fi;
	@if [ ! -d $(OBJ_DIR) ];     then mkdir -p $(OBJ_DIR);     fi;
	@if [ ! -d $(BIN_DIR) ];     then mkdir -p $(BIN_DIR);     fi;
	@if [ ! -d $(TEST_DIR) ];    then mkdir -p $(TEST_DIR);    fi;

clean:
	@rm -rf $(BIN_DIR)/*
	@rm -rf $(OBJ_DIR)/*
	@rm -f $(TEST_DIR)/*.o
	@rm -f $(TEST_APP)

publish:
	@echo "Exporting to dev environment"
	@if [ -f $(BIN_APP) ];  then cp $(BIN_APP)  $(DEV_BIN_DIR)/$(PKG);      fi;
	@if [ -f $(AR_TARG) ];  then cp $(AR_TARG)  $(DEV_LIB_DIR)/$(AR_FILE);  fi;
	@if [ -f $(SO_TARG) ];  then cp $(SO_TARG)  $(DEV_LIB_DIR)/$(SO_FILE);  fi;
	@if [ -f $(DYN_TARG) ]; then cp $(DYN_TARG) $(DEV_LIB_DIR)/$(DYN_FILE); fi;
	@if [ ! -d $(DEV_INC_DIR) ]; then mkdir -p $(DEV_INC_DIR); fi;
	@cp $(SRC_DIR)/*.h $(DEV_INC_DIR)/

unpublish:
	@echo "Cleaning up dev environment"
	@rm -f $(DEV_BIN_DIR)/$(PKG)
	@rm -f $(DEV_LIB_DIR)/$(AR_FILE)
	@rm -f $(DEV_LIB_DIR)/$(SO_FILE)
	@rm -f $(DEV_LIB_DIR)/$(DYN_FILE)
	@rm -rf $(DEV_INC_DIR)

install-bin: app
	@if [ -f $(BIN_APP) ]; then cp $(BIN_APP) $(SYS_BIN_DIR)/$(PKG); fi;

# Assumes the developer has already built precisely those libraries to install
install-lib:
	@if [ -f $(AR_TARG) ];  then cp $(AR_TARG)  $(SYS_LIB_DIR)/$(AR_FILE);  fi;
	@if [ -f $(SO_TARG) ];  then cp $(SO_TARG)  $(SYS_LIB_DIR)/$(SO_FILE);  fi;
	@if [ -f $(DYN_TARG) ]; then cp $(DYN_TARG) $(SYS_LIB_DIR)/$(DYN_FILE); fi;
	@if [ ! -d $(SYS_INC_DIR) ]; then mkdir -p $(SYS_INC_DIR); fi;
	@cp $(SRC_DIR)/*.h $(SYS_INC_DIR)

# Cleans up everything.
uninstall:
	@rm -rf $(SYS_INC_DIR)
	@rm -f $(SYS_LIB_DIR)/$(AR_FILE)
	@rm -f $(SYS_LIB_DIR)/$(SO_FILE)
	@rm -f $(SYS_LIB_DIR)/$(DYN_FILE)
	@rm -f $(SYS_BIN_DIR)/$(PKG)

test: $(TEST_APP)
	@$(TEST_APP)

tq: $(TEST_APP)
	@$(TEST_APP) -q

# -------------------------------------------------------------------------- #
# (7) Build Targets.                                                         #
# -------------------------------------------------------------------------- #

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	$(CC_EXEC) $(INCLUDES) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC_EXEC) $(INCLUDES) -c $< -o $@

$(BIN_APP): $(OBJ_FILES) $(BIN_SRC)
	$(CC_EXEC) $(OBJ_FILES) $(BIN_SRC) $(INCLUDES) $(LINK_LINE) -o $@

$(AR_TARG): $(OBJ_FILES)
	$(AR) $(AR_FLAGS) $@ $(OBJ_FILES)

$(SO_TARG): $(OBJ_FILES)
	$(CC_EXEC) $(LIB_FLAGS) $(OBJ_FILES) -o $@

$(DYN_TARG): $(OBJ_FILES)
ifeq ($(PLATFORM),$(APPLE))
		$(CC_EXEC) $(DYN_FLAGS) $(OBJ_FILES) -o $@
else
		@echo "Build dylib files only on MacOS"
endif

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c $(TEST_DIR)/%.h
	$(CC_EXEC) $(INCLUDES) -c $< -o $@

$(TEST_DIR)/%.o: $(TEST_DIR)/%.c
	$(CC_EXEC) $(INCLUDES) -c $< -o $@

$(TEST_APP): $(OBJ_FILES) $(TEST_OBJECTS) $(TEST_BIN_SRC)
	$(CC_EXEC) $(OBJ_FILES) $(TEST_OBJECTS) $(TEST_BIN_SRC) $(INCLUDES) $(LINK_LINE) -o $@

# -------------------------------------------------------------------------- #
# True elegance is the manifestation of an independent mind.                 #
# -------------------------------------------------------------------------- #
