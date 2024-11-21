# FROM alpine AS builder
FROM --platform=linux/amd64 ubuntu:20.04
ENV DEBIAN_FRONTEND=noninteractive

# RUN apk update && apk add --no-cache binutils make libgcc musl-dev gcc g++ asio-dev

RUN apt-get update && apt-get install -y \
    build-essential \
    # libgcc-dev \
    libasio-dev \
    && rm -rf /var/lib/apt/lists/*


WORKDIR /code
COPY . .
RUN make static

#FROM scratch AS final
#COPY --from=builder /code/ddd-backend /bin/ddd-backend
EXPOSE 4242
CMD ["/code/ddd-backend", "4242"]
