.PHONY: all

all: dependencies compile

dependencies:
	git clone https://github.com/clibplus/clibplus.git
	cd clibplus
	make
	cd ..
	rm -r clibplus
	pm -i str
	pm -i arr
	pm -i map
	pm -i Net
	mv /usr/local/lib/Net/* /usr/local/lib/
	rm -rf /usr/local/lib/Net

compile:
	cd ~/
	git clone https://github.com/clibplus/Net.git
	cd Net
	gcc -c web.c web_config.c html_n_css_gen.c html_parser.c wjs_gen.c -lstr -larr -lmap -lpthread -g -g1
	ar rcs websign.a *.o; rm *.o; mv websign.a /usr/local/lib/libwebsign.a
	cd ..
	rm -r Net
	echo You can now compile your app using '-lwebsign -lstr -larr -lmap -lpthread'