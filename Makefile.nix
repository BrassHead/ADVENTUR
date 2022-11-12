# Something quick and dirty... you must put things where you want...
#
all: munge
	cd src/adv ; make
	src/munge comcave
	@echo "All done."

clean:
	rm -f munge kio-effic
	cd src/adv ; rm -f adv alib.a *.o
	rm -f adv.key adv.rec

kio-effic: kio-effic.cpp kio.h
	c++ --std=c++11 -Wno-format-security -o kio-effic kio-effic.cpp

munge: munge.cpp kio.h fio.h
	c++ --std=c++11 -Wno-format-security -o munge munge.cpp
