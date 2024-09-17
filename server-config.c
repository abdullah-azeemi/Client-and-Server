// server-config.c
#include <stdio.h>
#include "server-config.h"

// Initialize configuration variables
int max_clients = MAX_THREADS;
int encryption_algorithm = ENCRYPTION_ALGO_HUFFMAN;

// Function to load configuration (placeholder for future file-based or environment-based config)
void load_configuration() {
    // For now, we just use hardcoded settings
    // In the future, you could read these from a configuration file or environment variables
    max_clients = MAX_THREADS;
    encryption_algorithm = ENCRYPTION_ALGO_HUFFMAN;
}

// Function to print current configuration
void print_configuration() {
    printf("Server Configuration:\n");
    printf("Max Clients: %d\n", max_clients);
    printf("Encryption Algorithm: %s\n",
           encryption_algorithm == ENCRYPTION_ALGO_HUFFMAN ? "Huffman Coding" : "None");
}
