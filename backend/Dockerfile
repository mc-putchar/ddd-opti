FROM alpine
RUN apk update && apk add --no-cache binutils musl-dev make gcc g++ libgcc
WORKDIR /code
COPY . .
RUN make static

#FROM scratch AS final
#COPY --from=builder /code/ddd-backend /bin/ddd-backend
EXPOSE 4242
CMD ["/code/ddd-backend", "4242"]
