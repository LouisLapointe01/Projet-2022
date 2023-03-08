CC = gcc
OPT = -Wall

couche1.o : couche1/couche1.c
	$(CC) -o couche1.o -c couche1/couche1.c $(OPT)

couche2.o : couche2/couche2.c
	$(CC) -o couche2.o -c couche2/couche2.c $(OPT)

couche3.o : couche3/couche3.c
	$(CC) -o couche3.o -c couche3/couche3.c $(OPT)

couche4.o : couche4/couche4.c
	$(CC) -o couche4.o -c couche4/couche4.c $(OPT)

couche5.o : couche5/couche5.c
	$(CC) -o couche5.o -c couche5/couche5.c $(OPT)


sha256_utils.o : Sha256/sha256_utils.c
	$(CC) -o sha256_utils.o -c Sha256/sha256_utils.c $(OPT)

sha256.o : Sha256/sha256.c
	$(CC) -o sha256.o -c Sha256/sha256.c $(OPT)

cmd_format.o : Format/cmd_format.c
	$(CC) -o cmd_format.o -c Format/cmd_format.c $(OPT)


disk : cmd_format.o
	$(CC) $(OPT) cmd_format.o -o cmd_format



sos_disk : disk couche1.o couche2.o couche3.o couche4.o couche5.o sha256_utils.o sha256.o
	$(CC) $(OPT) couche1.o couche2.o couche3.o couche4.o couche5.o sha256_utils.o sha256.o -o sos_disk


clean :
	rm -rf *.o *.exe *.stackdump

uninstall : clean
	rm -rf rep cmd_format



