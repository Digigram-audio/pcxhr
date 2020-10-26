#!/bin/sh

PCXHR_MODULE="pcxhr"
PCXHR_VERSION="2.0.12"

echo "****************************************"
echo "move sources"
echo "****************************************"
sudo cp -Rf ./ "/usr/src/${PCXHR_MODULE}-${PCXHR_VERSION}"

echo "****************************************"
echo "add dmks project"
echo "****************************************"
sudo dkms add -m "${PCXHR_MODULE}" -v "${PCXHR_VERSION}"

echo "****************************************"
echo "build module"
echo "****************************************"
#sudo dkms clean -m ${PCXHR_MODULE} -v "${PCXHR_VERSION}"
sudo dkms build -m "${PCXHR_MODULE}" -v "${PCXHR_VERSION}"

echo "****************************************"
echo "make package"
echo "****************************************"
sudo dkms mkdeb -m "${PCXHR_MODULE}" -v "${PCXHR_VERSION}"

echo "****************************************"
echo "get generated package"
echo "****************************************"
cp "/var/lib/dkms/${PCXHR_MODULE}/${PCXHR_VERSION}/deb/${PCXHR_MODULE}-dkms_${PCXHR_VERSION}_"*".deb" .

echo "****************************************"
echo "spring-clean"
echo "****************************************"
sudo dkms remove -m "${PCXHR_MODULE}" -v "${PCXHR_VERSION}" --all
sudo rm -Rf "/usr/src/${PCXHR_MODULE}-${PCXHR_VERSION}"

echo "****************************************"
echo "that's all folks."
echo "****************************************"
