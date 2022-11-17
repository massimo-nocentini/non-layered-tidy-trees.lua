
linux:
	gcc -O2 -std=gnu99 -fPIC -Wall -I/usr/local/include/ -s -shared -o libluanonlayeredtidytrees.so liblua-non-layered-tidy-trees.c -L/usr/local/lib/ -llua -lnonlayeredtidytrees

install:
	mkdir -p /usr/local/lib/lua/5.4/	# just for ensuring that the dest dir exists
	mkdir -p /usr/local/share/lua/5.4/	# just for ensuring that the dest dir exists
	mv libluanonlayeredtidytrees.so /usr/local/lib/lua/5.4/
	cp non-layered-tidy-trees.lua /usr/local/share/lua/5.4/

test:
	cd ../test && lua test.lua #> README
