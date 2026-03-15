# Custom Concurrent Memory Allocator (C)

## A multi-tier memory allocator designed to minimize lock contention in multi-threaded  environments.
### Architecture

* Thread-Local Cache (TLC): Lock-free fast-path allocation using the __thread storage class for 128 size classes (8B to 1024B).

* Central Cache: Shared global pool with fine-grained pthread_mutex locking for thread-cache refilling.

* Page-Level Management: Uses mmap() to request 4KB pages from the kernel, which are then carved into fixed-size blocks.
