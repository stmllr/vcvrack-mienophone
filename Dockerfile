FROM alpine:3.13

ENV http_dir=/srv/www
ENV api_dir=${http_dir}/face/v1.0
RUN apk add busybox-extras
RUN mkdir -p ${api_dir}
RUN echo '{ "anger": 0.001, "contempt": 0, "disgust": 0, "fear": 0.991, "happiness": 0, "neutral": 0, "sadness": 0.005, "surprise": 0.002 }' > ${api_dir}/detect
RUN echo '.json:application/json' >> /etc/httpd.conf

USER nobody
EXPOSE 8080
CMD httpd -p 8080 -f -h ${http_dir} -c /etc/httpd.conf
