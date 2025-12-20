# Create virtual linux port

To create virtaul port in linux systems you can use

```bash
socat -d -d pty,raw,echo=0 pty,raw,echo=0

socat pty,raw,echo=0,link=tmp/hardware_proto_manager/tty20 pty,raw,echo=0,link=tmp/hardware_proto_manager/ttyS21

```

Full documentation on socat [socat](https://man7.org/linux/man-pages/man1/socat.1.html)
