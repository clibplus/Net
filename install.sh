#!/usr/sh

sudo apt update -y && apt upgrade -y
sudo apt install make git gcc -y
sudo apt install libssl-dev -y

git clone https://github.com/clibplus/str.git
cd str
sudo gcc -c str.c utils.c -g -g3 -ggdb
sudo ar rcs str.a *.o; sudo mv str.a /usr/local/lib/libstr.a; sudo rm *.o
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
sudo gcc -c file.c -g -g3 -ggdb
sudo ar rcs file.a *.o; sudo mv file.a /usr/local/lib/libfile.a; sudo rm *.o
sudo mkdir -p /usr/local/include/OS
sudo cp file.h /usr/local/include

sudo gcc -c utils.c -g -g3 -ggdb
sudo ar rcs utils.a *.o; sudo mv utils.a /usr/local/lib/libutils.a; sudo rm *.o
sudo cp utils.h /usr/local/include
cd ..

git clone https://github.com/clibplus/Net.git
cd Net
sudo gcc -c socket.c -lpthread
sudo ar rcs socket.a *.o; sudo rm *.o; sudo mv socket.a /usr/local/lib/libsocket.a
sudo mkdir -p /usr/local/include/Net
sudo cp socket.h /usr/local/include/Net

sudo gcc -c ssl.c -lssl -lcrypto
sudo ar rcs ssl.a *.o; sudo rm *.o; sudo mv ssl.a /usr/local/lib/libssl.a
sudo cp ssl.h /usr/local/include/Net

sudo gcc -c request.c -lssl -lcrypto -lstr -larr
sudo ar rcs request.a *.o; sudo rm *.o; sudo mv request.a /usr/local/lib/librequest.a
sudo cp request.h /usr/local/include/Net

sudo gcc -c web.c web_config.c web_route.c websign/*.c -lstr -larr -lmap -lpthread -g -g1
sudo ar rcs websign.a *.o; sudo rm *.o; sudo mv websign.a /usr/local/lib/libwebsign.a
cp web.h /usr/local/include/Net