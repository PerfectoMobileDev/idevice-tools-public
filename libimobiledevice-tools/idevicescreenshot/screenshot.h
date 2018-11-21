#ifndef __SCREENSHOT_H__
#define __SCREENSHOT_H__


#include <libimobiledevice/screenshotr.h>

screenshotr_error_t get_screenshot_data(screenshotr_client_t shotr,char **imgdata,uint64_t *imgsize);


#endif
