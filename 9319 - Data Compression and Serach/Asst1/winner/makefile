Project = huffman

Sources = Decoder.cpp Encoder.cpp HuffmanTree.cpp main.cpp

RootDir := $(abspath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))

Mode = Release
OS = $(shell uname)

CXXFLAGS = -std=c++14 -pthread -Wall -Wno-dangling-else

ifeq ($(Mode),Debug)
    CXXFLAGS += -g -D_DEBUG
else ifeq ($(Mode),Release)
    CXXFLAGS += -O3
else
    $(error Mode must be either Debug or Release.)
endif

CXX := /usr/bin/clang++

MKDIR	:= mkdir -p

OutputDir = $(RootDir)/$(Mode)
DepDir = $(OutputDir)/depend/$(Project)
ObjDir = $(OutputDir)/obj/$(Project)

Objects = $(addprefix $(ObjDir)/, $(Sources:.cpp=.o))

IncludePath = $(addprefix -I$(RootDir)/, $(Dependencies)) $(addprefix -I, $(Include))

LinkerName             :=$(CXX)
LinkOptions            :=
OutputFile             :=$(Project)
Libs                   := $(addprefix -l, $(Dependencies) $(Libraries))

.PHONY: all clean
all: $(ObjDir) $(OutputFile)

$(OutputFile): $(Objects)
	@$(MKDIR) $(@D)
	$(LinkerName) -o$(OutputFile) $(Objects) $(LibPath) $(Libs) $(LinkOptions)

clean:
	$(RM) $(ObjDir)/*
	$(RM) $(OutputFile)

$(ObjDir):
	@test -d $(ObjDir) || $(MKDIR) $(ObjDir)

$(shell mkdir -p $(DepDir) >/dev/null)
DEPFLAGS = -MT $@ -MMD -MP -MF $(DepDir)/$*.Td

COMPILE.cc = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(IncludePath) -c
POSTCOMPILE = mv -f $(DepDir)/$*.Td $(DepDir)/$*.d ; touch $@

%.o : %.cpp
$(ObjDir)/%.o : %.cpp
$(ObjDir)/%.o : %.cpp $(DepDir)/%.d
	$(COMPILE.cc) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DepDir)/%.d: ;
.PRECIOUS: $(DepDir)/%.d

-include $(patsubst %,$(DepDir)/%.d,$(basename $(Sources)))
