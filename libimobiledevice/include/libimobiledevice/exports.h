#ifdef WIN32
#define LIBIMOBILEDEVICE_API __declspec( dllexport )
#else 
#define LIBIMOBILEDEVICE_API
#endif