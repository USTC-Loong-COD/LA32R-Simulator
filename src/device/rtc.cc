#include <mmio.h>
#include <sys/time.h>

static uint64_t boot_time = 0;

static uint64_t get_time_internal() {
    struct timeval now;
    gettimeofday(&now, NULL);
    uint64_t us = now.tv_sec * 1000000 + now.tv_usec;
    return us;
}

uint64_t get_time() {
    if (boot_time == 0) boot_time = get_time_internal();
    uint64_t now = get_time_internal();
    return now - boot_time;
}

struct tm *get_time_tm() {
    time_t now = time(NULL);
    return localtime(&now);
}

static void rtc_io_handler(void *base, uint32_t offset, uint32_t len, bool is_write) {
    assert(!is_write);
    uint64_t us = get_time();
    struct tm* rtc = get_time_tm();
    uint32_t *rtc_base = (uint32_t *)base;
    rtc_base[0] = (uint32_t)us;
    rtc_base[1] = (uint32_t)(us >> 32);
    rtc_base[2] = rtc->tm_sec;
    rtc_base[3] = rtc->tm_min;
    rtc_base[4] = rtc->tm_hour;
    rtc_base[5] = rtc->tm_mday;
    rtc_base[6] = rtc->tm_mon + 1;
    rtc_base[7] = rtc->tm_year + 1900;
}

void init_rtc(){
    register_io_map("rtc", CONFIG_RTC_MMIO, 32, new uint8_t[32], rtc_io_handler);
}