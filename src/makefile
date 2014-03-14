IDIR =../include
CXX=g++
CXXFLAGS=-I$(IDIR) `pkg-config --cflags --libs opencv`

ODIR=obj
LDIR =../lib

LIBS=-lm 

_DEPS = 
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = merge_images.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CXXFLAGS)

images_merger: $(OBJ)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS) 

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o *~ core $(INCDIR)/*~
