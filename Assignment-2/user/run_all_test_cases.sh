rm -f ./init.c
rm -f ./init.o
for file in init*.c; do
	rm -f ./init.o
	cp $file init.c
	make -C ../ > /dev/null
	rm init.c
	echo "Setup done for $file"
	r='y'
	read -p "Run gemOS now. Press 'y' once done, 'q' to quit [y]: " -n 1 r
	echo ""
	if [ $r == 'q' ]; then
		exit
	fi
done
