# SHELL := $(NEMU_HOME)/time_report.sh

.DEFAULT_GOAL = app

# Add necessary options if the target is a shared library
ifeq ($(SHARE),1)
SO = -so
CFLAGS  += -fPIC -Ibuild -fvisibility=hidden
LDFLAGS += -shared -fPIC
endif

WORK_DIR  = $(shell pwd)
BUILD_DIR = $(WORK_DIR)/build

INC_PATH := $(WORK_DIR)/include $(INC_PATH)
OBJ_DIR  = $(BUILD_DIR)/obj-$(NAME)$(SO)
EXPR_ENG_DIR = $(BUILD_DIR)/exp-eng
BINARY   = $(BUILD_DIR)/$(NAME)$(SO)

# Compilation flags
ifeq ($(CC),clang)
CXX := clang++
else
CXX := g++
endif
LD := $(CXX)
LEX := flex
YACC := bison
INCLUDES = $(addprefix -I, $(INC_PATH))
CFLAGS  := -O2 -MMD -Wall -Werror $(INCLUDES) $(CFLAGS)
LDFLAGS := -O2 -flto $(LDFLAGS)

LEX_C = $(LEXES:%.l=%.yy.c)
PARSER_C = $(YACCS:%.y=%.tab.c)
OBJS = $(SRCS:%.c=$(OBJ_DIR)/%.o) $(CXXSRCS:%.cpp=$(OBJ_DIR)/%.o)

# Compilation patterns

%.yy.c: %.l
	@echo + LEX $<
	@$(LEX) -o $@ $<

%.tab.c %.tab.h: %.y
	@echo + YACC $<
	@$(YACC) -d -o $@ $<

$(OBJ_DIR)/%.o: %.c
	@echo + CC $<
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<
	$(call call_fixdep, $(@:.o=.d), $@)

$(OBJ_DIR)/%.o: %.cpp
	@echo + CXX $<
	@mkdir -p $(dir $@)
	@$(CXX) $(CFLAGS) $(CXXFLAGS) -c -o $@ $<
	$(call call_fixdep, $(@:.o=.d), $@)

# Depencies
-include $(OBJS:.o=.d)

# Some convenient rules

.PHONY: app clean

flexbison: $(LEX_C) $(PARSER_C)

app:  $(BINARY) 

$(BINARY):: $(OBJS) $(ARCHIVES) 
	@echo + LD $@
	@$(LD) -o $@ $(OBJS) $(LDFLAGS) $(ARCHIVES) $(LIBS)

clean:
	-rm -rf $(BUILD_DIR)

clean_exp_eng:
	-find . -name "*.yy.c" -exec rm {} \;
	-find . -name "*.tab.c" -exec rm {} \;
	-find . -name "*.tab.h" -exec rm {} \;

clean_all: clean clean_exp_eng

