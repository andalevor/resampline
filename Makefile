EXECUTABLE=resampline

SDIR = src
IDIR = $(SDIR)/include

CC := g++

ifeq ($(BUILD),)
	ODIR = build
else
	ODIR = $(BUILD)
endif

ifeq ($(ODIR), debug)
	CXXFLAGS := ${CXXFLAGS} --std=c++11 -Wall -Wextra -g -fsanitize=address -fsanitize=undefined -I$(IDIR)
else ifeq ($(ODIR), gperftools)
	CXXFLAGS := ${CXXFLAGS} --std=c++11 -Wall -Wextra -g -DWITHPERFTOOLS -Ofast -I$(IDIR)
	LDFLAGS = -lprofiler
else
	CXXFLAGS := ${CXXFLAGS} --std=c++11 -DNDEBUG -Ofast -I$(IDIR)
endif

SRC = $(wildcard $(SDIR)/*.cpp)
DEP = $(wildcard $(IDIR)/*.hpp)
OBJ = $(patsubst $(SDIR)/%.cpp, $(ODIR)/%.o, $(SRC))

all: directories $(EXECUTABLE)

$(ODIR)/%.o: $(SDIR)/%.cpp $(DEP)
	$(CC) $(CXXFLAGS) -c $< -o $@

$(EXECUTABLE): $(OBJ)
	$(CC) $(CXXFLAGS) $(OBJ) -o $(EXECUTABLE)

.PHONY: clean

clean:
	rm -rf $(ODIR)
	rm -f $(EXECUTABLE)

directories: ${ODIR}

${ODIR}:
	mkdir -p ${ODIR}
