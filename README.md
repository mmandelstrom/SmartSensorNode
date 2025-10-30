# Lightweight TCP Client in C

This project implements a small reusable TCP client written in C.  
It demonstrates:

Dynamic memory handling  
Pointer-to-pointer usage (`TCPClient**`)  
Function pointers for callback-based data processing  
Clean resource management (RAII-style dispose functions)  
HTTP request example using a raw TCP socket  

The client connects to a remote server, sends a request, receives data dynamically, and passes the data to a user-defined callback.

---

## Features

- TCP connect using hostname + port

- Auto-growing receive buffer

- User callback for incoming data:

  ```c
  typedef void (*TCPClient_DataHandler)(const char* data, size_t len, void* ctx);
  ```

- Pointer-to-pointer creation (`TCPClient_InitiatePtr`)

- Clean teardown (`TCPClient_DisposePtr`)

- Example HTTP POST to `httpbin.org`

---

## File Structure

```
/project-root
 ├── TCPClient.h
 ├── TCPClient.c
 ├── main.c
 └── Makefile
```

---

## Build

This repository uses a Makefile configured to build the client target.

### Quick build (release by default)

```bash
make
```

### Build modes

The build mode is controlled by the `MODE` variable (default: `release`).

```bash
# Debug build (extra warnings, symbols)
make MODE=debug

# Clean + fresh debug build
make clean && make MODE=debug

# Parallel build
make -j
```

### Debug helper

A convenience target is provided for running the client under gdb in debug mode:

```bash
make debug-client
```

This target will:

- Clean the tree
- Rebuild with `MODE=debug`
- Launch `gdb client` and run it

### Compile a single file (utility target)
For WSL some devpackages might be missing. If you get a linker error run the following:
sudo apt update
sudo apt install -y build-essential pkg-config uuid-dev libcurl4-openssl-dev libbsd-dev

You can compile one source file with the same flags/defines as the client build:

```bash
make compile FILE=path/to/source.c
```

> The object will be placed under `build/client/…` mirroring the source tree.

### Outputs

- Executable: `./client`
- Objects: `build/client/**/*.o`

---

## Run

```bash
./client
```

Expected output example:

```
Sending request
POST /post HTTP/1.1
Host: httpbin.org
...

[Callback] Received 200 bytes:
HTTP/1.1 200 OK
...
```

---

## Code Example

**Register callback + send HTTP request**

```c
TCPClient_InitiatePtr(&client, "httpbin.org", "80", PrintHandler, NULL);
client->writeBuffer = malloc(strlen(request)+1);
strcpy(client->writeBuffer, request);
TCPClient_Write(client, strlen(request));
TCPClient_Read(client);
TCPClient_DisposePtr(&client);
```

## 
