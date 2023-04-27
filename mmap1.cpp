#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

int main() {
    // Step 1: Open the file and get the file descriptor
    int fd = open("file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Step 2: Get the file size
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }
    size_t file_size = st.st_size;

    // Step 3: Create the memory-mapped file
    void *addr = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // Step 4: Access the contents of the file as a memory buffer
    const char *buffer = static_cast<const char *>(addr);

    // Now you can use the buffer to read the file contents
    // For example, print the contents to stdout
    std::cout.write(buffer, file_size);
    std::cout << std::endl;

    // Step 5: Unmap the file and close the file descriptor
    if (munmap(addr, file_size) == -1) {
        perror("munmap");
    }
    close(fd);

    return 0;
}
