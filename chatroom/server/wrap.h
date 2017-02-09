#ifndef WRAP_H
#define WRAP_H

void perr_exit(const char *s);

int Accept(int fd, struct  sockaddr *sa, socklen_t *salenptr);

void Bind(int fd, const struct sockaddr * sa, socklen_t salen);

void Connet(int fd, const struct sockaddr *sa, socklen_t salen);

void Listen(int fd, int backlog);

int Socket(int family, int type, int protocol);

ssize_t Read(int  fd, void *ptr, size_t nbyte);

ssize_t Write(int fd, void *ptr, size_t nbyte);

void Close(int fd);

ssize_t Readn(int fd, void *ptr, size_t n);

ssize_t Writen(int fd, void *ptr, size_t n);

static size_t my_read(int fd, char *ptr);

ssize_t ReadLine(int fd, void *vptr, size_t maxlen);

#endif
