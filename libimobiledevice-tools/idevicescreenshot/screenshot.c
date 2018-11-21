//#include <winsock2.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>


#include "screenshot.h"


screenshotr_error_t get_screenshot_data(screenshotr_client_t shotr,char **imgdata,uint64_t *imgsize)
{
	screenshotr_error_t res;

			
	res = screenshotr_take_screenshot(shotr, imgdata, imgsize);

	if (res != SCREENSHOTR_E_SUCCESS)
	{
		printf("Could not get screenshot!\n");
	}


	return res;
}
