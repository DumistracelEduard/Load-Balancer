// Copyright 2021 Dumistracel Eduard-Costin
#include <stdlib.h>
#include <string.h>

#include "server.h"
#include "utils.h"

server_memory* init_server_memory() {
	server_memory* server = malloc(sizeof(server_memory));
	DIE(server ==  NULL, "Error\n");

	server->server_table = ht_create(200, hash_function_key,
							compare_function_strings);

	server->server_hash = 0;
	server->server_tag = 0;

	return server;
}

void server_store(server_memory* server, char* key, char* value) {
	ht_put(server->server_table, key, strlen(key) + 1, value, strlen(value) + 1);
}

void server_remove(server_memory* server, char* key) {
	ht_remove_entry(server->server_table, key);
}

char* server_retrieve(server_memory* server, char* key) {
	return ht_get(server->server_table, key);
}

void free_server_memory(server_memory* server) {
	ht_free(server->server_table);
	free(server);
}
