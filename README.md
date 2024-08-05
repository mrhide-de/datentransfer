# Datentransfer

Test setup to send 32bit data packages from a ESP to a server

- ESP8266 - NodeMCU ESP-12E
- Python 3 Server

data types: int, float

## Output

### Terminal (Python 3)

```
### NEW VALUES ### (2 sets)
===========================
TYPE f
VALUE 3.1415927410125732
ID pi
===========================
TYPE i
VALUE -53
ID wlan
===========================
```

### Serial monitor (Arduino IDE)

```
Sending :6640490fdb706920204269ffffffce776c616e40 - 2 sets
```

## Struct pattern

A set cotains 10 bytes in 20 hex characters

- type: 1 byte (char; 'i' or 'f')
- value: 4 bytes (int OR float)
- id: 4 bytes (char[4])
- null: 1 byte

## Run test

- Insert your wifi credentials in tcp-client.ino
- Both server and client files need to have the same ip address for the python server
- Navigate your terminal to the folder python-server.
- Run the python file with `python3 server.py`
- Connect ESP to your computer and observe the serial port on the IDE of your choice

## TODOs

### Client

- Make struct array dynamic
- Adding a contructor for struct
- Better way of serializing a char[4]
- Test edge cases

### Server

- Deal with corrupt sets or hexs
- Test edge cases
