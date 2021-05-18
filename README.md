# VCV Plugin for Mienophone


## Dependencies

* build-essential
* libcurl-dev

```
apt install libcurl4-openssl-dev build-essential
```

## Build and Installation

export RACK_DIR=$HOME/Projects/vcvrack/Rack-SDK

```
make
make dist
make install
```

## Fake API

```
docker build . -t fake-api
docker run -it --rm -d --name fake-api -p 8080:8080 fake-api
curl -i http://localhost:8080/face/v1.0/detect
```
