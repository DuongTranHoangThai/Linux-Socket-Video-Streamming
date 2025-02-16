#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12346
#define CHUNK_SIZE 1024  // Adjust this based on network performance

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    FILE *file;
    char buffer[CHUNK_SIZE];

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Set up destination address (VLC listener)
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Send to localhost

    // Open the video file
    file = fopen("video.mp4", "rb");
    if (!file) {
        perror("Failed to open video file");
        close(sockfd);
        return 1;
    }

    // Read and send file in chunks
    while (!feof(file)) {
        size_t bytes_read = fread(buffer, 1, CHUNK_SIZE, file);
        if (bytes_read > 0) {
            sendto(sockfd, buffer, bytes_read, 0, 
                   (struct sockaddr*)&server_addr, sizeof(server_addr));
            usleep(100); // Add small delay to prevent overwhelming the network
        }
    }
    
    printf("Streaming completed.\n");

    fclose(file);
    close(sockfd);
    return 0;
}
