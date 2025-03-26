#!/usr/sh

sudo apt update -y && apt upgrade -y

git clone https://github.com/clibplus/str.git
cd str
gcc -c str.c utils.c -g -g3 -ggdb
ar rcs str.a *.o; mv str.a /usr/local/lib/libstr.a; rm *.o
cp str.h /usr/local/include
cd ..

git clone https://github.com/clibplus/arr.git
cd arr
sudo gcc -c arr.c -g -g3 -ggdb
sudo ar rcs arr.a *.o;sudo  mv arr.a /usr/local/lib/libarr.a; sudo  rm *.o
cp arr.h /usr/local/include
cd ..

git clone https://github.com/clibplus/map.git
cd map
sudo gcc -c map.c json.c -g -g3 -ggdb
sudo ar rcs map.a *.o; sudo mv map.a /usr/local/lib/libmap.a; sudo rm *.o
cp map.h /usr/local/include
cd ..

git clone https://github.com/clibplus/OS.git
cd OS
gcc -c file.c -g -g3 -ggdb
ar rcs file.a *.o; mv file.a /usr/local/lib/libfile.a; rm *.o
mkdir -p /usr/local/include/OS
cp file.h /usr/local/include

gcc -c utils.c -g -g3 -ggdb
ar rcs utils.a *.o; mv utils.a /usr/local/lib/libutils.a; rm *.o
cp utils.h /usr/local/include
cd ..

git clone https://github.com/clibplus/Net.git
cd Net
gcc -c socket.c -lpthread
ar rcs socket.a *.o; rm *.o; mv socket.a /usr/local/lib/libsocket.a
mkdir -p /usr/local/include/Net
cp socket.h /usr/local/include/Net

gcc -c ssl.c -lssl -lcrypto
ar rcs ssl.a *.o; rm *.o; mv ssl.a /usr/local/lib/libssl.a
cp ssl.h /usr/local/include/Net

gcc -c request.c -lssl -lcrypto -lstr -larr
ar rcs request.a *.o; rm *.o; mv request.a /usr/local/lib/librequest.a
cp request.h /usr/local/include/Net

gcc -c web.c web_config.c web_route.c websign/*.c -lstr -larr -lmap -lpthread -g -g1
ar rcs websign.a *.o; rm *.o; mv websign.a /usr/local/lib/libwebsign.a
cp web.h /usr/local/include/Net