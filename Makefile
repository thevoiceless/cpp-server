CXX = g++
CXXFLAGS = -g -pthread
OBJ_FILES = server.o

server: ${OBJ_FILES}
	${CXX} ${CXXFLAGS} ${OBJ_FILES} -o $@ ${LIBRARIES}

clean:
	rm -f server ${OBJ_FILES}