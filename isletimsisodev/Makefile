# G231210355 Metehan Gülşer
# B231210355 Emir Kır
# G231210387 Yiğit Alp Çakır
# B211210006 Muhammet Enes Nas
# B221210003 Akif Emre Yaman


HEADERS = main.h
OBJECTS = main.o

default: main

%.o: %.c $(HEADERS)
	gcc -c $< -o $@

main: $(OBJECTS)
	gcc $(OBJECTS) -o $@

run: main
	./main