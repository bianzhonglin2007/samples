CC := gcc

#LOCAL_SRC_FILES := ./auto_ptr.cpp

LOCAL_SRC_FILES := ./malloc.c

LOCAL_MODULE := malloc_test

#LOCAL_CFLAGS :=  -std=c++0x \

all:
	$(CC) $(LOCAL_CFLAGS) $(LOCAL_SRC_FILES) -o ${LOCAL_MODULE}

clean:
	@rm -rf ${LOCAL_MODULE}
