FROM alpine:3.13

RUN apk add ruby ruby-webrick ruby-etc ruby-json
COPY fake_api.rb .

USER nobody
EXPOSE 8080
CMD ruby fake_api.rb
