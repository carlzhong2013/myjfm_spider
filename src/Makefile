#Program, flags, etc.
CC			= g++
CFLAGS	= -O2 -Wall -c -g -I../deps/adns/include -I../deps/zlib/include
LEX			= flex
LD			= g++
LDFLAGS	= -lpthread -D_REENTRANT

LINK_LEXER	= link_lexer.cc
URI_LEXER	= uri_lexer.cc
OBJS		= myjfm_spider.o utility.o log.o global.o shared.o thread_pool.o \
					thread.o thread_task.o logger.o url.o md5.o dns_cache.o \
					downloader_task.o extractor_task.o scheduler_task.o dnser_task.o\
					logger_task.o event_loop.o memory_pool.o link_lexer.o uri_lexer.o
BIN			= myjfm_spider

UTILITY_UT_OBJS	= utility.o utility_ut.o
UTILITY_UT_BIN	= utility_ut

THREAD_TEST_OBJS	= thread_test.o utility.o log.o global.o shared.o url.o \
										thread_pool.o thread.o thread_task.o logger.o \
										md5.o dns_cache.o downloader_task.o extractor_task.o \
										scheduler_task.o dnser_task.o logger_task.o event_loop.o \
										memory_pool.o link_lexer.o uri_lexer.o
THREAD_TEST_BIN	= thread_test

HASH_TEST_OBJS	= hash_test.o utility.o log.o global.o shared.o url.o \
										thread_pool.o thread.o thread_task.o logger.o \
										md5.o dns_cache.o downloader_task.o extractor_task.o \
										scheduler_task.o dnser_task.o logger_task.o event_loop.o \
										memory_pool.o link_lexer.o uri_lexer.o
HASH_TEST_BIN	= hash_test

MD5_TEST_OBJS	= md5_test.o utility.o log.o global.o shared.o url.o \
										thread_pool.o thread.o thread_task.o logger.o \
										md5.o dns_cache.o downloader_task.o extractor_task.o \
										scheduler_task.o dnser_task.o logger_task.o event_loop.o \
										memory_pool.o link_lexer.o uri_lexer.o
MD5_TEST_BIN	= md5_test

URL_TEST_OBJS	= url_test.o utility.o log.o global.o shared.o url.o \
										thread_pool.o thread.o thread_task.o logger.o \
										md5.o dns_cache.o downloader_task.o extractor_task.o \
										scheduler_task.o dnser_task.o logger_task.o event_loop.o \
										memory_pool.o link_lexer.o uri_lexer.o
URL_TEST_BIN	= url_test

export OUTPUT_PATH=../output

.PHONY : everything clean realclean all

everything : $(BIN) $(UTILITY_UT_BIN) $(THREAD_TEST_BIN) $(HASH_TEST_BIN) \
	$(MD5_TEST_BIN) $(URL_TEST_BIN) others

all : clean everything

clean : 
	rm -rf $(OBJS)
	rm -rf $(UTILITY_UT_OBJS)
	rm -rf $(THREAD_TEST_OBJS)
	rm -rf $(HASH_TEST_OBJS)
	rm -rf $(MD5_TEST_OBJS)
	rm -rf $(URL_TEST_OBJS)
	rm -rf $(LINK_LEXER)
	rm -rf $(URI_LEXER)

realclean : 
	rm -rf $(OBJS) $(BIN)
	rm -rf $(OUTPUT_PATH)/$(BIN)
	rm -rf $(UTILITY_UT_OBJS) $(UTILITY_UT_BIN)
	rm -rf $(THREAD_TEST_OBJS) $(THREAD_TEST_BIN)
	rm -rf $(HASH_TEST_OBJS) $(HASH_TEST_BIN)
	rm -rf $(MD5_TEST_OBJS) $(MD5_TEST_BIN)
	rm -rf $(URL_TEST_OBJS) $(URL_TEST_BIN)
	rm -rf $(LINK_LEXER)
	rm -rf $(URI_LEXER)

$(BIN) : $(OBJS)
	$(LD) $(LDFLAGS) -o $(BIN) $(OBJS) \
		../deps/adns/lib/libadns.a ../deps/zlib/lib/libz.a

$(UTILITY_UT_BIN) : $(UTILITY_UT_OBJS)
	$(LD) $(LDFLAGS) -o $(UTILITY_UT_BIN) $(UTILITY_UT_OBJS) \
		../deps/adns/lib/libadns.a ../deps/zlib/lib/libz.a

$(THREAD_TEST_BIN) : $(THREAD_TEST_OBJS)
	$(LD) $(LDFLAGS) -o $(THREAD_TEST_BIN) $(THREAD_TEST_OBJS) \
		../deps/adns/lib/libadns.a ../deps/zlib/lib/libz.a

$(HASH_TEST_BIN) : $(HASH_TEST_OBJS)
	$(LD) $(LDFLAGS) -o $(HASH_TEST_BIN) $(HASH_TEST_OBJS) \
		../deps/adns/lib/libadns.a ../deps/zlib/lib/libz.a

$(MD5_TEST_BIN) : $(MD5_TEST_OBJS)
	$(LD) $(LDFLAGS) -o $(MD5_TEST_BIN) $(MD5_TEST_OBJS) \
		../deps/adns/lib/libadns.a ../deps/zlib/lib/libz.a

$(URL_TEST_BIN) : $(URL_TEST_OBJS)
	$(LD) $(LDFLAGS) -o $(URL_TEST_BIN) $(URL_TEST_OBJS) \
		../deps/adns/lib/libadns.a ../deps/zlib/lib/libz.a

others : 
	cp $(BIN) $(OUTPUT_PATH)/
	cp $(UTILITY_UT_BIN) $(OUTPUT_PATH)/
	cp $(THREAD_TEST_BIN) $(OUTPUT_PATH)/
	cp $(HASH_TEST_BIN) $(OUTPUT_PATH)/
	cp $(MD5_TEST_BIN) $(OUTPUT_PATH)/
	cp $(URL_TEST_BIN) $(OUTPUT_PATH)/

myjfm_spider.o : myjfm_spider.cc
	$(CC) $(CFLAGS) -o $@ $<

global.o : global.cc
	$(CC) $(CFLAGS) -o $@ $<

hash_test.o : hash_test.cc
	$(CC) $(CFLAGS) -o $@ $<

utility.o : utility.cc
	$(CC) $(CFLAGS) -o $@ $<

utility_ut.o : utility_ut.cc
	$(CC) $(CFLAGS) -o $@ $<

shared.o : shared.cc
	$(CC) $(CFLAGS) -o $@ $<

thread.o : thread.cc
	$(CC) $(CFLAGS) -o $@ $<

thread_pool.o : thread_pool.cc
	$(CC) $(CFLAGS) -o $@ $<

thread_task.o : thread_task.cc
	$(CC) $(CFLAGS) -o $@ $<

thread_test.o : thread_test.cc
	$(CC) $(CFLAGS) -o $@ $<

url.o : url.cc
	$(CC) $(CFLAGS) -o $@ $<

url_test.o : url_test.cc
	$(CC) $(CFLAGS) -o $@ $<

md5.o : md5.cc
	$(CC) $(CFLAGS) -o $@ $<

md5_test.o : md5_test.cc
	$(CC) $(CFLAGS) -o $@ $<

dns_cache.o : dns_cache.cc
	$(CC) $(CFLAGS) -o $@ $<

downloader_task.o : downloader_task.cc
	$(CC) $(CFLAGS) -o $@ $<

extractor_task.o : extractor_task.cc
	$(CC) $(CFLAGS) -o $@ $<

scheduler_task.o : scheduler_task.cc
	$(CC) $(CFLAGS) -o $@ $<

dnser_task.o : dnser_task.cc
	$(CC) $(CFLAGS) -o $@ $<

log.o : log.cc
	$(CC) $(CFLAGS) -o $@ $<

logger.o : logger.cc
	$(CC) $(CFLAGS) -o $@ $<

logger_task.o : logger_task.cc
	$(CC) $(CFLAGS) -o $@ $<

event_loop.o : event_loop.cc
	$(CC) $(CFLAGS) -o $@ $<

memory_pool.o : memory_pool.cc
	$(CC) $(CFLAGS) -o $@ $<

link_lexer.o : link_lexer.l
	$(LEX) -o $(LINK_LEXER) $<
	$(CC) $(CFLAGS) -o $@ $(LINK_LEXER)

uri_lexer.o : uri_lexer.l
	$(LEX) -o $(URI_LEXER) $<
	$(CC) $(CFLAGS) -o $@ $(URI_LEXER)

