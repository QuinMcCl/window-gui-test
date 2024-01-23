LIB_DIR = $(PWD)/external
DEPS = nonstd nonstd_glfw_opengl nonstd_imgui
LIB_DIRS =     $(foreach d, $(DEPS), $(LIB_DIR)/$d) $(LIB_DIR)/glew 
LIB_INCLUDES = $(foreach d, $(LIB_DIRS), $d/include)

LIBSCLEAN=$(addsuffix clean,$(LIB_DIRS))
LIBSfCLEAN=$(addsuffix fclean,$(LIB_DIRS))
LIBSALL=$(addsuffix all,$(LIB_DIRS))

EXE_NAME = main

INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

EXE = $(BIN_DIR)/$(EXE_NAME)
SRC = $(wildcard $(SRC_DIR)/*.c )
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

export GLEW_NO_GLU = -DGLEW_NO_GLU
export PYTHON ?= python3
export CPPFLAGS = -Iinclude -MMD -MP -DERROR_CHECKING -DGLEW_NO_GLU
CFLAGS   = -Wall -Wextra -Werror -g
LDFLAGS  = $(foreach d, $(LIB_DIRS), -L $d/lib)
LDLIBS   = $(foreach d, $(DEPS), -l$d) -lpthread -lGL -l:libGLEW.a -lglfw -lm
INCLUDES = $(foreach d, $(LIB_INCLUDES), -I$d)

.PHONY: all clean  fclean re
all: $(LIBSALL) $(EXE) 

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@ $(INCLUDES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c  | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@ 

$(BIN_DIR) $(OBJ_DIR):
	mkdir -p $@

clean: $(LIBSCLEAN)
	$(RM) -rv $(BIN_DIR) $(OBJ_DIR)

fclean: $(LIBSfCLEAN) clean
	$(RM) -f $(EXE)

re: fclean | $(EXE)

%clean: %
	$(MAKE) -C $< clean

%fclean: %
	$(MAKE) -C $< fclean

%all: %
	$(MAKE) -C $< all

-include $(OBJ:.o=.d)