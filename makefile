# Determine the platform
UNAME_S := $(shell uname -s)

# CC
ifeq ($(UNAME_S),Darwin)
    CC := clang++ -arch x86_64
else
    CC := g++
endif

# Folders
SRCDIR := src
BUILDDIR := build
TARGETDIR := bin

# Targets
EXECUTABLE := contour
TARGET := $(TARGETDIR)/$(EXECUTABLE)

# Final Paths
INSTALLBINDIR := install

# Code Lists
SRCEXT := cpp
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

# Folder Lists
INCDIRS := $(shell find include/* -name '*.h' -exec dirname {} \; | sort | uniq)
INCLIST := $(patsubst include/%,-I include/%,$(INCDIRS))
BUILDLIST := $(patsubst include/%,$(BUILDDIR)/%,$(INCDIRS))

# Shared Compiler Flags
CFLAGS := -c -Wall -g -std=c++17
INC := -I include $(INCLIST) -I /usr/local/include -I /Users/brahimboudamouz/Perso/dev/libraries/opencv/include/opencv4 -I /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/
LIB := -L /usr/local/lib -L /Users/brahimboudamouz/Perso/dev/libraries/opencv/lib -L /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_imgcodecs -lcurl


# Platform Specific Compiler Flags
ifeq ($(UNAME_S),Linux)
    CFLAGS += -O2 # -fPIC
else
    CFLAGS += -stdlib=libc++ -O2
endif

$(TARGET): $(OBJECTS)
	@mkdir -pv $(TARGETDIR)
	@echo "linking $(TARGET)"; $(CC) $^ -o $(TARGET) $(LIB)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDLIST)
	@echo "compiling $< ... "; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	$(RM) -r $(BUILDDIR)/* $(TARGET)

install:
	cp $(TARGET) $(INSTALLBINDIR)

distclean:
	rm $(INSTALLBINDIR)/$(EXECUTABLE)

.PHONY: clean
