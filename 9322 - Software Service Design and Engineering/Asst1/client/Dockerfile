FROM alpine:latest
RUN apk add --no-cache python3-dev
RUN pip3 install --upgrade pip
WORKDIR /service
COPY ./client /service
EXPOSE 5002
RUN pip3 install -r requirements.txt
CMD python3 __init__.py