#!/bin/sh

# Build the DKMS packages for Debian/Ubuntu and Fedora/CentOS
for i in /lib/modules/* ; do
	dkms build pcxhr/${PCXHR_VERSION} -k $(basename $i) && \
	dkms mkrpm pcxhr/${PCXHR_VERSION} -k $(basename $i) && \
	dkms mkdeb pcxhr/${PCXHR_VERSION} -k $(basename $i)
done

mkdir -p "${OUTPUT_DIR}" && \
cp /var/lib/dkms/pcxhr/${PCXHR_VERSION}/rpm/pcxhr-${PCXHR_VERSION}-1dkms.noarch.rpm ${OUTPUT_DIR} && \
cp /var/lib/dkms/pcxhr/${PCXHR_VERSION}/deb/pcxhr-dkms_${PCXHR_VERSION}_amd64.deb ${OUTPUT_DIR}

