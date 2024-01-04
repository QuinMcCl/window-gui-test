LIB_DIR = $(PWD)/external
DEPS = nonstd nonstd_glfw_opengl 
LIB_DIRS =     $(foreach d, $(DEPS), $(LIB_DIR)/$d)  $(LIB_DIR)/imgui
LIB_INCLUDES = $(foreach d, $(DEPS), $(LIB_DIR)/$d/include) $(PWD)/include $(LIB_DIR)/imgui $(LIB_DIR)/imgui/backends

LIBSCLEAN=$(addsuffix clean,$(LIB_DIRS))
LIBSfCLEAN=$(addsuffix fclean,$(LIB_DIRS))
LIBSALL=$(addsuffix all,$(LIB_DIRS))

EXE_NAME = main

INC_DIR = include
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

EXE = $(BIN_DIR)/$(EXE_NAME)
SRC = $(wildcard $(SRC_DIR)/*.cpp )
OBJ = $(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)


export CPPFLAGS = -Iinclude -MMD -MP -Ofast -DERROR_CHECKING
CFLAGS   = -Wall -Wextra -Werror -g
LDFLAGS  = $(foreach d, $(LIB_DIRS), -L $d/bin)
LDLIBS   = $(foreach d, $(DEPS), -l$d) -lpthread -lglfw -lGLEW -lm -limgui
INCLUDES = $(foreach d, $(LIB_INCLUDES), -I$d)

.PHONY: all clean  fclean re
all: $(LIBSALL) $(EXE) 

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@ $(INCLUDES)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp  | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) $(CFLAGS) $(INCLUDES) -c $< -o $@ 

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