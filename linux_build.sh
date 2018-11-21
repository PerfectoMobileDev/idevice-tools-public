#!/bin/sh

set +x

hss_ver=$1
export HSS_VERSION=$hss_ver
make


