/**
 * @file mediacodec.cpp
 * @brief 
 * @author YOUR NAME (), 
 * @version 1.0
 * @history
 * 		鍙傝 :
 * 		2012-11-21 YOUR NAME created
 */

#include <stdio.h>
#include <string.h>
#include <cutils/memory.h>

#include <unistd.h>
#include <utils/Log.h>

#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>
#include <binder/IServiceManager.h>
#include <media/stagefright/foundation/ADebug.h>

#include "NdkMediaCodec.h"
#include "NdkMediaExtractor.h"

#include "yuv2rgb.h"

#define LOG_TAG "mediacodec_test"

using namespace android;

AMediaCodec *codec = NULL;
long start_time = 0;

int startCodec(int w, int h)
{
    media_status_t status;
    if((codec = AMediaCodec_createCodecByName("OMX.hisi.video.decoder.avc")) == NULL)
    {
        ALOGE("AMediaCodec_createCodecByName failed! \n");
        return -1;
    }
    AMediaFormat* format = AMediaFormat_new();
    if(format == NULL)
    {
        ALOGE("AMediaFormat_new failed! \n");
        return -1;
    }
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_WIDTH, w );
    AMediaFormat_setInt32(format, AMEDIAFORMAT_KEY_HEIGHT, h);	
    AMediaFormat_setString(format, AMEDIAFORMAT_KEY_MIME, "video/avc");	
    status = AMediaCodec_configure(codec, format, NULL, NULL, 2);           
    status = AMediaCodec_start(codec);   

    return 0;
}

int64_t systemnanotime() {
    timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec * 1000000000LL + now.tv_nsec;
}

void doCodecWork(char* src_buf, int len, int index)
{
    if(codec == NULL)
    {
        ALOGD("codec is null!");
        return ;
    }
    ssize_t bufidx = -1;
    int64_t presentationNano = systemnanotime();  //start time
    bufidx = AMediaCodec_dequeueInputBuffer(codec, -1);
    //ALOGD("input buffer %zd", bufidx);
    if (bufidx >= 0) {
        size_t bufsize;
        uint8_t *buf = AMediaCodec_getInputBuffer(codec, bufidx, &bufsize);
        ALOGD("input bufsize %d, len:%d \n", bufsize, len);
        // 把src_buf拷贝到buf中
        memcpy(buf, src_buf, len);
        long int presentationTimeUs = 1000 * 1000;
        AMediaCodec_queueInputBuffer(codec, bufidx, 0, len, presentationTimeUs, 0);
    }
    AMediaCodecBufferInfo info;
    ssize_t status = AMediaCodec_dequeueOutputBuffer(codec, &info, -1);
    if (status >= 0) {
        if (info.flags & AMEDIACODEC_BUFFER_FLAG_END_OF_STREAM) {
            ALOGD("output EOS");          
        }
        size_t out_size = 0;
        uint8_t *yuv = AMediaCodec_getOutputBuffer(codec, status, &out_size );
        AMediaCodec_releaseOutputBuffer(codec, status, info.size != 0);
        ALOGD("bzl ++++ new decoder time: %lld \n", (systemnanotime() - presentationNano) / 1000);  //end time
        /*--------------------------保存yuv数据到文件------------------------- */
        FILE * fp = NULL;
        char yuv_file[128];
        snprintf(yuv_file, sizeof(yuv_file), "/cache/yuv_%d.yuv", index);
        if((fp = fopen(yuv_file, "wb")) == NULL)
        {
            ALOGE("fopen %s failed!\n", yuv_file);
            return ;
        }
        fwrite(yuv, 1, out_size, fp);
        fclose(fp);
        fp = NULL;
        /*--------------------------------------------------------------- */

        /*-------------------------yuv数据转换为rgb数据------------------ */
        char rgb_file[128];
        snprintf(rgb_file, sizeof(rgb_file), "/cache/rgb_%d.ppm", index);
        if((fp = fopen(rgb_file, "wb")) == NULL)
        {
            ALOGE("fopen %s failed!\n", yuv_file);
            return ;
        }
        char* rgb_buf = (char*)malloc(1920 * 1080 * 3);
        if(rgb_buf == NULL)
        {
            ALOGE("rgb buf malloc failed\n");
            return ;
        }
        presentationNano = systemnanotime();
        bool ret = nv21_to_rgb((unsigned char *)rgb_buf, (unsigned char const*)yuv, 1920, 1080);        
        ALOGD("bzl ++++ nv21_to_rgb time: %lld \n", (systemnanotime() - presentationNano) / 1000);

        fprintf(fp, "P6\n%d %d\n255\n", 1920, 1080);
        fwrite(rgb_buf, 1, 1920 * 1080 * 3, fp);
        fclose(fp);
        fp = NULL;
        /*--------------------------------------------------------------- */


    } else if (status == AMEDIACODEC_INFO_OUTPUT_BUFFERS_CHANGED) {
        ALOGD("output buffers changed");
    } else if (status == AMEDIACODEC_INFO_OUTPUT_FORMAT_CHANGED) {
        AMediaFormat *format = NULL;
        format = AMediaCodec_getOutputFormat(codec);
        ALOGD("format changed to: %s", AMediaFormat_toString(format));
        AMediaFormat_delete(format);
    } else if (status == AMEDIACODEC_INFO_TRY_AGAIN_LATER){
        ALOGD("no outpu buffer righ now");
    } else {
        ALOGD("unexpected info code: %zd", status);
    }

    return ;
}


int main(int argc, char * argv[])
{
    // set up the thread-pool  
    sp<ProcessState> proc(ProcessState::self());  
    ProcessState::self()->startThreadPool(); 

    startCodec(1920, 1080);

    char file[128] = {"/sdcard/h264/dump-0.h264"};

    FILE* fd = NULL;
    int num = 10;
    int index =0;
    while(index < num)
    {
        char t = '0' + index;
        file[18] = t;
        //ALOGD("++++file: %s\n", file);
        fd = fopen(file, "rb");		
        if (fd == NULL)
        {
            ALOGD("open file: %s error\n", file);
            return -1;
        }
        char *buf = (char*)malloc(1024 * 1024);
        int len = fread(buf, 1, 1024*1024, fd);
        //ALOGD("index: %d, fread lend: %d\n", index, len);
        doCodecWork(buf, len, index);
        fclose(fd);
        index++;
    }

    //保持进程一直运行
    IPCThreadState::self()->joinThreadPool();//可以保证画面一直显示，否则瞬间消失  
    IPCThreadState::self()->stopProcess();

    return 0;
}
