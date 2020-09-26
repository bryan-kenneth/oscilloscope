INCLUDEVC=-I /opt/vc/include -I /usr/include
INCLUDEUSB=-I /usr/include/libusb-1.0/
OBJECTS=main.o cmdargs.o usbcomm.o i2ccomm.o graphics.o data.o
FLAGS=-lshapes -lm -lusb-1.0 -lwiringPi

myscope: main.o cmdargs.o usbcomm.o i2ccomm.o graphics.o data.o
	gcc -o myscope $(OBJECTS) $(FLAGS)

main.o: main.c scope.h
	gcc -c main.c $(INCLUDEVC)

cmdargs.o: cmdargs.c scope.h
	gcc -c cmdargs.c $(INCLUDEVC)

usbcomm.o: usbcomm.c scope.h
	gcc -c usbcomm.c $(INCLUDEVC) $(INCLUDEUSB)

i2ccomm.o: i2ccomm.c scope.h
	gcc -c i2ccomm.c $(INCLUDEVC)

graphics.o: graphics.c scope.h
	gcc -c graphics.c $(INCLUDEVC)

data.o: data.c scope.h
	gcc -c data.c $(INCLUDEVC)

clean: 
	rm *.o 


    

