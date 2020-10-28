FROM centos:8

# Set this to the relevant Git tag
ENV PCXHR_VERSION="2.0.12"
# Built image will be moved here. This should be a host mount to get the output.
ENV OUTPUT_DIR="/output"

# Prepare the system with required packages
RUN 	dnf install -y epel-release && \
	dnf install -y 'dnf-command(config-manager)' && \
	dnf config-manager --set-enabled PowerTools && \
	dnf install -y \
		kernel-core \
		kernel-devel \
		dkms \
		rpm-build \
		fakeroot \
		debhelper

# Add the source code and a required DKMS config file
ADD ./src /usr/src/pcxhr-${PCXHR_VERSION}
ADD ./.docker/etc/dkms/template-dkms-mkdeb /etc/dkms/template-dkms-mkdeb

# Working directory setup
ADD ./.docker/workdir /workdir

# Building script
ENTRYPOINT ["/workdir/build-dkms.sh"]
