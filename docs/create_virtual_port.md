# Create virtual linux port

To create virtaul port in linux systems you can use

```bash
socat -d -d pty,raw,echo=0 pty,raw,echo=0
```

Full documentation on socat (socat)[https://man7.org/linux/man-pages/man1/socat.1.html]