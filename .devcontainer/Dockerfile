FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-24.04

# Update and install GCC 14
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    gcc-14 g++-14 \
    && apt-get clean

# Set GCC 14 as default (optional)
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

ADD --checksum=sha256:2e7cdf358526e8937b36b93fc96c501b8d6a0c8436e95e65440de31c48ec3086 https://archives.boost.io/beta/1.87.0.beta1/source/boost_1_87_0_b1.tar.bz2 /opt/boost_1_87_0_b1.tar.bz2
RUN tar xjf /opt/boost_1_87_0_b1.tar.bz2 -C /opt
WORKDIR /opt/boost_1_87_0
RUN ./bootstrap.sh && ./b2 && ./b2 install
RUN rm -rf /opt/boost_1_87_0 /opt/boost_1_87_0_b1.tar.bz2

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>
