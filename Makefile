#
# Makefile
# Project 4
#
#  Created by Phillip Romig on 4/3/12.
#  Copyright 2012 Colorado School of Mines. All rights reserved.
#

CXX = g++
CXXFLAGS = -g -pthread
OBJ_FILES = project1.o

project1: ${OBJ_FILES}
	${CXX} ${CXXFLAGS} ${OBJ_FILES} -o $@ ${LIBRARIES}

clean:
	rm -f project1 ${OBJ_FILES}

submit:
	rm -f project1 ${OBJ_FILES}
	mkdir rimoses
	cp Makefile README.txt *.h *.cc rimoses
	tar cf rimoses.tar rimoses
	rm -rf rimoses