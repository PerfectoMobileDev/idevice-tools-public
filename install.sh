#!/bin/sh

set +x

#when installing libssl with apt-get
#there is no ssl2 enabled, so tools will not work on ios 8
#while we need to support ios8, we need ssl2 enabled
install_libssl()
{
	openssl_version=1.0.2k
	wget https://openssl.org/source/openssl-$openssl_version.tar.gz
	tar -xvf openssl-$openssl_version.tar.gz
	cd openssl-$openssl_version
	./Configure shared threads zlib-dynamic --prefix=/usr --openssldir=/etc/ssl -m32 linux-generic32
	make depend
	make
	sudo rm -f /lib/i386-linux-gnu/libssl.*
	sudo rm -f /lib/i386-linux-gnu/libcrypto.*
	sudo cp libssl.so.1.0.0 /lib/i386-linux-gnu/libssl.so.1.0.0
	sudo cp libcrypto.so.1.0.0 /lib/i386-linux-gnu/libcrypto.so.1.0.0
	cd /lib/i386-linux-gnu
	sudo ln -s libssl.so.1.0.0 libssl.so
	sudo ln -s libcrypto.so.1.0.0 libcrypto.so
	cd -
	rm -rf openssl-$openssl_version
	rm openssl-$openssl_version.tar.gz
}

sudo apt-get -y update
sudo apt-get -y install zlib1g-dev
sudo apt-get -y install libssl-dev
sudo apt-get -y install libxml2-dev
sudo apt-get install g++-multilib
sudo apt-get install gcc-multilib
mkdir Release
mkdir Debug
#sudo apt-get install lib32ncurses5 lib32gomp1 lib32z1-dev
#sudo apt-get install lib32gcc1 libc6-i386 lib32z1 lib32stdc++6
#sudo install_libssl 
#Set patterns for core dump names and path
mkdir -p $HOME/root/idevice-tools/Dumps
sudo sh -c "echo '$HOME/root/idevice-tools/Dumps/core.%e.%p.%h.%t' > /proc/sys/kernel/core_pattern"
sudo sh -c "echo 'enabled=0' > /etc/default/apport"
sudo sysctl kernel.core_pattern=$HOME/root/idevice-tools/Dumps/core.%e.%p.%h.%t
sudo sh -c "echo 'kernel.core_pattern=$HOME/root/idevice-tools/Dumps/core.%e.%p.%h.%t' > /etc/sysctl.d/60-core-pattern.conf"


