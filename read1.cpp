#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <vector>

constexpr size_t BUFFER_SIZE = 4096;

int main() {
    // Step 1: Open the file and get the file descriptor
    int fd = open("file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Step 2: Read the file in smaller chunks using a loop
    std::vector<char> buffer(BUFFER_SIZE);
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer.data(), BUFFER_SIZE)) > 0) {
        // Step 3: Process the chunk as needed
        // In this example, we'll print the chunk to stdout
        std::cout.write(buffer.data(), bytes_read);
    }

    // Check for errors
    if (bytes_read == -1) {
        perror("read");
        close(fd);
        return 1;
    }

    // Step 4: Close the file descriptor
    close(fd);

    return 0;
}

