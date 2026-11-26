FROM ubuntu:24.04
RUN apt-get update && apt-get install -y --no-install-recommends \
	build-essential \
    cmake \
    ninja-build \
    git \
    curl \
    zip \
    unzip \
    tar \
    pkg-config \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*
ENV VCPKG_ROOT=/opt/vcpkg

RUN git clone https://github.com/microsoft/vcpkg.git ${VCPKG_ROOT} \
    && ${VCPKG_ROOT}/bootstrap-vcpkg.sh -disableMetrics

WORKDIR /app

COPY . .

RUN cmake -S . -B build -G Ninja \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_TOOLCHAIN_FILE=${VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake \
    && cmake --build build --target booking_service

EXPOSE 8080

CMD ["./build/booking_service"]