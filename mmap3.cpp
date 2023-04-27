#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

// Function to process a segment of the memory-mapped file and count characters/bytes
void process_segment(const char *buffer, size_t start, size_t end, std::atomic<size_t> &count) {
    for (size_t i = start; i < end; ++i) {
        // Increment the count for each character/byte in the segment
        count++;
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

    // Create the memory-mapped file
    void *addr = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    // Access the contents of the file as a memory buffer
    const char *buffer = static_cast<const char *>(addr);

    // Determine the number of threads and the size of each segment
    unsigned int num_threads = std::thread::hardware_concurrency();
    size_t segment_size = file_size / num_threads;

    // Create a vector to store the character/byte count for each thread
    std::vector<std::atomic<size_t>> count_per_thread(num_threads);

    // Create threads and assign each thread a segment of the memory-mapped file
    std::vector<std::thread> threads;
    for (unsigned int i = 0; i < num_threads; ++i) {
        size_t start = i * segment_size;
        size_t end = (i == num_threads - 1) ? file_size : (start + segment_size);
        threads.emplace_back(process_segment, buffer, start, end, std::ref(count_per_thread[i]));
    }

    // Join the threads after they finish processing
    for (auto &t : threads) {
        t.join();
    }

    // Unmap the file and close the file descriptor
    if (munmap(addr, file_size) == -1) {
        perror("munmap");
    }
    close(fd);

    // Calculate the total character/byte count
    size_t total_count = 0;
    for (const auto &count : count_per_thread) {
        total_count += count;
    }

    std::cout << "Total character/byte count: " << total_count << std::endl;

    return 0;
}

