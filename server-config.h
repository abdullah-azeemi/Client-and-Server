// server-config.h
#ifndef SERVER_CONFIG_H
#define SERVER_CONFIG_H

// Configuration settings
#define MAX_THREADS 10
#define ENCRYPTION_ALGO_HUFFMAN 1
#define ENCRYPTION_ALGO_NONE 0

// Global configuration variables
extern int max_clients;
extern int encryption_algorithm;

// Function declarations
void load_configuration();
void print_configuration();

#endif // SERVER_CONFIG_H
