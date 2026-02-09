#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

/* 
 * This function handles the communication with a specific client.
 * It is called in a new thread for every connection.
 */
void* client_handler(void* socket_ptr) {
    int client_fd = *(int*)socket_ptr;
    free(socket_ptr); // Clean up the memory allocated in main()
    
    char buffer[1024];

    // Allocate 1MB for session processing
    char *session_data = (char*)malloc(1024 * 1024); 
    
    if (session_data == NULL) {
        perror("Could not allocate session memory");
        close(client_fd);
        return NULL;
    }

    // Initialize memory to ensure it's physically mapped
    memset(session_data, 0, 1024 * 1024);
    printf("[Thread %lu] Allocated 1MB at: %p\n", pthread_self(), (void*)session_data);

    // Wait for client to send a message
    int bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("[Thread %lu] Received: %s\n", pthread_self(), buffer);
    }

    /* --- THE FIX --- */
    // We must free the memory before the function returns to prevent a leak
    free(session_data); 
    printf("[Thread %lu] Memory freed successfully.\n", pthread_self());
    /* --------------- */

    close(client_fd);
    return NULL;
}

int main() {
    int server_fd, *new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size = sizeof(struct sockaddr_in);

    // Create the socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Could not create socket");
        return 1;
    }

    // Set socket options so we can restart the server without "Address already in use" errors
    int enable = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
    }

    // Configure the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8888);

    // Bind to the port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return 1;
    }

    // Listen for incoming connections
    listen(server_fd, 5);
    printf("Server is running on port 8888. Waiting for connections...\n");

    while (1) {
        // Accept a new connection
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_size);
        if (client_fd < 0) {
            perror("Accept failed");
            continue;
        }

        // Allocate memory for the socket ID to pass it to the thread safely
        new_sock = malloc(sizeof(int));
        if (new_sock == NULL) {
            perror("malloc failed");
            close(client_fd);
            continue;
        }
        *new_sock = client_fd;

        pthread_t tid;
        if (pthread_create(&tid, NULL, client_handler, (void*)new_sock) != 0) {
            perror("Could not create thread");
            free(new_sock);
            close(client_fd);
        }

        // Detach the thread so we don't have to join it later
        pthread_detach(tid);
    }

    close(server_fd);
    return 0;
}
