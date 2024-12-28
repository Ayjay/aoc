FROM mcr.microsoft.com/devcontainers/cpp:ubuntu-24.04

# Update and install GCC 14 and clang 20
RUN wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key | apt-key add -
RUN echo "deb http://apt.llvm.org/noble/ llvm-toolchain-noble main\ndeb-src http://apt.llvm.org/noble/ llvm-toolchain-noble main" >/etc/apt/sources.list.d/llvm.list
ARG DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get upgrade -y && apt-get install -y \
    gcc-14 g++-14 clang lld clang-format \
    && apt-get clean

# Set GCC 14 as default (optional)
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-14 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-14 100

ADD --checksum=sha256:af57be25cb4c4f4b413ed692fe378affb4352ea50fbe294a11ef548f4d527d89 https://boostorg.jfrog.io/artifactory/main/release/1.87.0/source/boost_1_87_0.tar.bz2 /opt/boost_1_87_0.tar.bz2
RUN tar xjf /opt/boost_1_87_0.tar.bz2 -C /opt
WORKDIR /opt/boost_1_87_0
RUN ./bootstrap.sh && ./b2 && ./b2 install
RUN rm -rf /opt/boost_1_87_0 /opt/boost_1_87_0_b1.tar.bz2

# [Optional] Uncomment this section to install additional vcpkg ports.
# RUN su vscode -c "${VCPKG_ROOT}/vcpkg install <your-port-name-here>"

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update && export DEBIAN_FRONTEND=noninteractive \
#     && apt-get -y install --no-install-recommends <your-package-list-here>
