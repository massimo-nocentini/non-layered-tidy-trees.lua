
linux:
	gcc -O2 -fPIC -Wall -I/usr/local/include/ -s -shared -o libluanonlayeredtidytrees.so liblua-non-layered-tidy-trees.c -L/usr/local/lib/ -llua -lnonlayeredtidytrees
	
mingw:
	gcc -O2 -fPIC -Wall -I/usr/local/include/ -s -shared -o libluanonlayeredtidytrees.dll liblua-non-layered-tidy-trees.c -L/usr/local/lib/ -llua -lnonlayeredtidytrees

test:
	gcc -I/usr/local/include/ -o libluanonlayeredtidytrees.dll liblua-non-layered-tidy-trees.c -L/usr/local/lib/ -llua -lnonlayeredtidytrees
	
install:
	mkdir -p /usr/local/lib/lua/5.4/	# just for ensuring that the dest dir exists
	mkdir -p /usr/local/share/lua/5.4/	# just for ensuring that the dest dir exists
	mv libluanonlayeredtidytrees.so /usr/local/lib/lua/5.4/
	cp non-layered-tidy-trees.lua /usr/local/share/lua/5.4/

mingw-install:
	mkdir -p /usr/local/lib/lua/5.4/	# just for ensuring that the dest dir exists
	mkdir -p /usr/local/share/lua/5.4/	# just for ensuring that the dest dir exists
	mv libluanonlayeredtidytrees.dll /usr/local/lib/lua/5.4/
	cp non-layered-tidy-trees.lua /usr/local/share/lua/5.4/

