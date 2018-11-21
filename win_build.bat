SET hss_ver=%1
SET CL=/DHSS_VERSION#%hss_ver%
"C:\\Program Files (x86)\\Microsoft Visual Studio 12.0\\Common7\\IDE\\devenv.com" libimobiledevice-win32.sln /build release
