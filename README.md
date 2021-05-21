# VCV Plugin for Mienophone

## Todo

- [ ] non-blocking module or API requests (Multithreading / Async / ...)
- [ ] webcam capture
- [ ] clock/gate input

## What?

![Rack module](res/Mienophone.png)

## What for?

https://mienophone.com/

## Build and Installation

### Dependencies

* build-essential
* libcurl-dev

```
apt install libcurl4-openssl-dev build-essential
```

### Build Plugin

export RACK_DIR=$HOME/Projects/vcvrack/Rack-SDK

```
make
make dist
make install
```

## Test

### Fake API

```
docker build . -t fake-api
docker run -it --rm -d --name fake-api -p 8080:8080 fake-api
curl -i http://localhost:8080/face/v1.0/detect
```
