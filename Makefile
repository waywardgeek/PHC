CC=gcc
CPP=g++
CFLAGS=-Wall -O3 -march=native -std=c99 -pthread -lcrypto -lm
#CFLAGS=-Wall -g -march=native -std=c99 -pthread -lcrypto -lm
CPPFLAGS=-Wall -O3 -march=native -pthread -lcrypto -lm

BATTCRYPT=Battcrypt/code/c++
CATENA=Catena/code/src
CATFISH=Catfish/ref-c
EARWORM=EARWORM
GAMBIT=Gambit/src
LANAREA=Lanarea
TORTUGA=Tortuga
TWOCATS=TwoCats/twocats

EXE=phs-antcrypt phs-argon phs-battcrypt phs-catena phs-catfish phs-centrifuge phs-earworm phs-gambit phs-lanarea phs-pomelo phs-schvrch phs-tortuga phs-twocats phs-yarn

all: $(EXE)

phs-antcrypt: main.c AntCrypt/source/phc.c
	$(CC) $(CFLAGS) -o $@ $^

phs-argon: main.c Argon/Reference_implementation/argon-ref.cpp
	$(CPP) $(CPPFLAGS) -o $@ main.c Argon/Reference_implementation/argon-ref.cpp

phs-battcrypt: main.c  $(BATTCRYPT)/battcrypt.cpp $(BATTCRYPT)/blowfish.cpp $(BATTCRYPT)/sha512.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

phs-catena: main.c $(CATENA)/catena.c $(CATENA)/catena-blake2b.c $(CATENA)/blake2/blake2b.c
	$(CC) $(CFLAGS)  -fgnu89-inline -I$(CATENA)/blake2 -o $@ $^

phs-catfish: main.c $(CATFISH)/catfish.c $(CATFISH)/KeccakF-1600-reference.c $(CATFISH)/KeccakSponge.c $(CATFISH)/displayIntermediateValues.c $(CATFISH)/pkhash_1024.c
	$(CC) $(CFLAGS) -o $@ $^

phs-centrifuge: main.c Centrifuge/cfuge.c
	$(CC) $(CFLAGS) -o $@ $^

phs-earworm: main.c $(EARWORM)/aes.c $(EARWORM)/core-opt.c $(EARWORM)/phc.c $(EARWORM)/sha256.c $(EARWORM)/util-opt.h
	$(CC) $(CFLAGS) -o $@ $^

phs-gambit: main.c $(GAMBIT)/gambit.cpp $(GAMBIT)/keccak.cpp
	$(CPP) $(CPPFLAGS) -o $@ $^

phs-lanarea: main.c $(LANAREA)/lanarea.c
	(cd $(LANAREA)/libb2; ./configure && make)
	$(CC) $(CFLAGS) -o $@ $^ $(LANAREA)/libb2/src/.libs/libb2_la-blake2b.o

phs-pomelo: main.c POMELO/pomelo.c
	$(CC) $(CFLAGS) -o $@ $^

phs-schvrch: main.c Schvrch/schvrch.c
	$(CC) $(CFLAGS) -o $@ $^

phs-tortuga: main.c $(TORTUGA)/turtle.c $(TORTUGA)/tortuga.c $(TORTUGA)/phs.c
	$(CC) $(CFLAGS) -o $@ $^

phs-twocats: main.c $(TWOCATS)/twocats-common.c $(TWOCATS)/twocats-blake2s.c $(TWOCATS)/twocats-blake2b.c $(TWOCATS)/twocats-sha256.c $(TWOCATS)/twocats-sha512.c $(TWOCATS)/twocats.c
	$(CC) $(CFLAGS) -o $@ $^

phs-yarn: main.c Yarn/yarn.c
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(EXE)
