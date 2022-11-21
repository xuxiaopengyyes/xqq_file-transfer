all : ser

GDB=-g

ser: ser.o socket.o work_thread.o
	gcc -o ser ser.o socket.o work_thread.o -lpthread

ser.o: ser.c
	gcc -c ser.c ${GDB}

socket.o: socket.c
	gcc -c socket.c ${GDB}

work_thread.o: work_thread.c
	gcc -c work_thread.c ${GDB}

clean:
	rm -rf *.o ser
