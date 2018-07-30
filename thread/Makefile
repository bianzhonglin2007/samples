CC := g++

LOCAL_SRC_FILES := thread.cpp

LOCAL_MODULE := thread_test

LOCAL_CFLAGS :=  -std=c++0x \
				 -pthread

all:
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_SRC_FILES)

clean:
	@rm -rf a.out
