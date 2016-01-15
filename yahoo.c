#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uv.h>

void
on_close(uv_handle_t *handle) {
    free(handle);
}

void
on_read(uv_stream_t *handle,
        ssize_t nread,
        const uv_buf_t* buf) {

    if (nread < 0) {
            uv_close((uv_handle_t *)handle, on_close);
    }

    if (nread < 0) {
        fwrite(buf->base, 1, nread, stdout);
    }

    free(buf->base);
}

void
on_alloc(uv_handle_t *handle,
        size_t size,
        uv_buf_t *buf) {
    buf->base = malloc(size);
    buf->len = size;
}

void
after_write(uv_write_t *req,
        int status) {
    free(req);
}

void
after_connect_cb(
        uv_connect_t *connect_req,
        int status) {

    uv_write_t *write_req;
    uv_buf_t buf;

    if (status < 0) {
        abort();
    }

    write_req = malloc(sizeof(uv_write_t));

    buf.base = "GET / HTTP/1.0\r\n"
               "Host: www.yahoo.com\r\n"
               "\r\n";
    buf.len = strlen(buf.base);

    uv_write(write_req, connect_req->handle, &buf, 1, after_write);

    uv_read_start(connect_req->handle, on_alloc, on_read);

    free(connect_req);

}

void
after_getaddrinfo_cb(
        uv_getaddrinfo_t *r,
        int status,
        struct addrinfo *ai) {
    uv_tcp_t *tcp_handle;
    uv_connect_t *connect_req;

    if (status < 0) {
        abort();
    }

    tcp_handle = malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), tcp_handle);

    connect_req = malloc(sizeof(uv_connect_t));
    uv_tcp_connect(connect_req,
            tcp_handle,
            ai->ai_addr,
            after_connect_cb);
    free(r);
    uv_freeaddrinfo(ai);
}

int
main(int argc, char **argv) {

    uv_getaddrinfo_t *r = malloc(sizeof(uv_getaddrinfo_t));

    uv_getaddrinfo(uv_default_loop(),
            r,
            after_getaddrinfo_cb,
            "www.yahoo.com",
            "80",
            NULL);

    printf("Staring\n");
    uv_run(uv_default_loop(), UV_RUN_DEFAULT);
    uv_loop_close(uv_default_loop());

    printf("Leaving\n");

    return 0;
}
