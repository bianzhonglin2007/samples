CC := gcc

LOCAL_SRC_FILES := setjmp.c

LOCAL_MODULE := setjmp_test

LOCAL_CFLAGS := -g -rdynamic

all:
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_SRC_FILES) -o $(LOCAL_MODULE)

clean:
	rm -rf ${LOCAL_MODULE}
