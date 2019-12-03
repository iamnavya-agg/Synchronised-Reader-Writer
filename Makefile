#makefile to compile semaphore code
CC=gcc  						#compiler
TARGET=semaphore_executable		#semaphore file name

all:    #target name
		$(CC) -o $(TARGET) semaphore.c -lpthread
clean:	#clean the history file
		rm $(TARGET)