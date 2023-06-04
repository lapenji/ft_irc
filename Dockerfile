# PER FARLO FUNZIONARE:
# docker build . -t ft_irc
# docker container run -p  6667:6667 ft_irc

FROM alpine:3.17.0 AS build
RUN apk update && \
	apk add --no-cache \
	build-base=0.5-r3
WORKDIR /ft_irc
EXPOSE 6667
COPY ./* ./
RUN make && make clean
CMD ./ircserv 6667 ciccio
