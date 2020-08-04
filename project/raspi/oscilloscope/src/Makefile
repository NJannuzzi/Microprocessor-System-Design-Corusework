myscope: main.o cmdargs.o comms.o graphics.o data.o
		gcc -g -o myscope main.o cmdargs.o comms.o graphics.o data.o -lshapes -lusb-1.0 -lwiringPi 
		
main.o: main.c comms.h data.h graphics.h cmdargs.h
		gcc -g -c main.c -I/usr/include/libusb-1.0/ -I/opt/vc/include -I /usr/include -lshapes -lusb-1.0
		
comms.o: comms.c comms.h
		gcc -g -c comms.c -I/usr/include/libusb-1.0/


cmdargs.o: cmdargs.c cmdargs.h
		gcc -g -c cmdargs.c


graphics.o: graphics.c graphics.h
		gcc -g -c graphics.c -I/opt/vc/include -I/usr/include/libusb-1.0/ -I /usr/include -lshapes
		
data.o: data.c data.h	graphics.h cmdargs.h	
		gcc -g -c data.c -I/opt/vc/include -I/usr/include/libusb-1.0/ -I /usr/include -lshapes -lusb-1.0
		
clean:
	rm *.o
		
