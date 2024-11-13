FROM alpine AS builder
RUN apk update && apk add --no-cache binutils make libgcc musl-dev gcc g++ asio-dev
WORKDIR /code
COPY . .
RUN make static

#FROM scratch AS final
#COPY --from=builder /code/ddd-backend /bin/ddd-backend
EXPOSE 4242
CMD ["/code/ddd-backend", "4242"]
