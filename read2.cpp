#include <iostream>
#include <vector>
#include <thread>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

constexpr size_t BUFFER_SIZE = 4096;

// Function to process a segment of the file
void process_segment(int fd, size_t start, size_t end) {
    // Set the file position for the current thread
    if (lseek(fd, start, SEEK_SET) == -1) {
        perror("lseek");
        return;
    }

    std::vector<char> buffer(BUFFER_SIZE);
    ssize_t bytes_read;

    for (size_t pos = start; pos < end; pos += bytes_read) {
        // Read the file in chunks
        size_t bytes_to_read = std::min(BUFFER_SIZE, end - pos);
        bytes_read = read(fd, buffer.data(), bytes_to_read);

        if (bytes_read <= 0) {
            perror("read");
            return;
        }

        // Process the chunk, e.g., count characters, search for a pattern, etc.
        // In this example, we print the chunk to stdout
        std::cout.write(buffer.data(), bytes_read);
    }
}

int main() {
    // Open the file and get the file descriptor
    int fd = open("large_file.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Get the file size
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }
    size_t file_size = st.st_size;

    // Determine the number of threads and the size of each segment
    unsigned int num_threads = std::thread::hardware_concurrency();
    size_t segment_size = file_size / num_threads;

    // Create threads and assign each thread a segment of the file
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i) {
        size_t start = i * segment_size;
        size_t end = (i == num_threads - 1) ? file_size : (start + segment_size);
        threads.emplace_back(process_segment, fd, start, end);
    }

    // Join the threads after they finish processing
    for (auto &t : threads) {
        t.join();
    }

    // Close the file descriptor
    close(fd);

    return 0;
}

