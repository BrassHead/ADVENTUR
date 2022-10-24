# Something quick and dirty... you must put things where you want...
#
all:
	cd kio
	nmake DEBUG=$(DEBUG)
	cd ..\munge
	nmake DEBUG=$(DEBUG)
	cd ..\adv
	nmake DEBUG=$(DEBUG)
	cd ..
	munge\munge.prg comcave
	echo all done

clean:
	cd kio
	nmake clean
	cd ..\munge
	nmake clean
	cd ..\adv
	nmake clean
	cd ..
	del adv.rec
	del adv.key

