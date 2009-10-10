all : kankeinai-bot

kankeinai-bot : kankeinai-bot.cc nanowww/nanowww.h picojson/picojson.h
	g++ -o kankeinai-bot -Inanowww/extlib/ -Inanowww -Ipicojson -I. kankeinai-bot.cc nanowww/extlib/picohttpparser/picohttpparser.c

nanowww/nanowww.h :
	git clone git://github.com/tokuhirom/nanowww.git
	(cd nanowww; git submodule init; git submodule update)

picojson/picojson.h :
	svn co http://svn.coderepos.org/share/lang/cplusplus/picojson/trunk picojson

clean :
	rm -f kankeinai-bot
	rm -rf nanowww
	rm -rf picojson
