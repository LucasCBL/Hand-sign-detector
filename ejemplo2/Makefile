CCX=g++

CXXFLAGS= -g


OPENCV= `pkg-config --cflags opencv` `pkg-config --libs opencv`

OBJS = *.cpp
all: ${OBJS}
	$(CCX) $(CXXFLAGS)   -o prueba ${OBJS} $(OPENCV)
