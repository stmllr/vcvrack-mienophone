# VCV Plugin for Mienophone

## Fake API

```
docker build . -t fake-api
docker run -it --rm -d --name fake-api -p 8080:8080 fake-api
curl -i http://localhost:8080/face/v1.0/detect
```
