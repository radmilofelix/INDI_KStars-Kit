#!/bin/bash
echo
echo



build()
{
	./buildroof.sh makeall
	./buildweather.sh makeall
	./buildfocuser.sh makeall
	./buildheater.sh makeall
	./buildflats.sh makeall
}

clean()
{
	./buildroof.sh cleanall
	./buildweather.sh cleanall
	./buildfocuser.sh cleanall
	./buildheater.sh cleanall
	./buildflats.sh cleanall
}

rebuild()
{
	clean
	build
}

help()
{
    echo
    echo `basename "$0"` help:
    echo "Usage: `basename "$0"` < build | clean | rebuild >"
    echo
}

if [ -z $1 ]
then
	help
fi

$1
