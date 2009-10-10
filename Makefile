all : kankeinai-bot

kankeinai-bot : kankeinai-bot.cc nanowww/nanowww.h picojson/picojson.h
	g++ -o kankeinai-bot -Inanowww/extlib/ -Inanowww -Ipicojson -I. kankeinai-bot.cc picohttpparser/picohttpparser.c

nanowww/nanowww.h :
	git clone git://github.com/tokuhirom/nanowww.git
	git clone git://github.com/kazuho/picohttpparser.git
	git clone git://github.com/tokuhirom/picouri.git

picojson/picojson.h :
	svn co http://svn.coderepos.org/share/lang/cplusplus/picojson/trunk picojson
