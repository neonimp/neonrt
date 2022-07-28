/**
 * @brief Key value store sample application.
 * @author Matheus Xavier <mxavier@neonlayer.com>
 * @copyright Copyright (c) 2022, Matheus Xavier
 */

#include <stdio.h>
#include <containers/hashmap.h>
#include <neon_info.h>

int read_line(neon_buff_t **cmdline_buff, char *prompt);

int main(void)
{
	hmap_t *hmap = NULL;
	neon_buff_t *cmdline_buff = NULL;
	neon_buff_t *key_buff = NULL;

	printf("NeonLayer KV Store\nLinked to NeonLayer %s\n", RT_VERSION_STRING);
	printf("Compiled with %s\n", neon_get_compiler());
	printf("\n");

	hmap = hmap_new(100, 75, 0, NULL);
	if (hmap == NULL) {
		printf("Error creating hashmap\n");
		return 1;
	}

	printf("Hashmap created with initial capacity of %zu\n", hmap->capacity);
	if (read_line(&cmdline_buff, "Enter command: ")) {
		printf("Error reading command\n");
		return 1;
	}

	while (buff_cmp_raw(cmdline_buff, ":q", 2) == false) {
		if (buff_cmp_raw(cmdline_buff, ":i", 2) == true) {
			printf("Hashmap capacity: %zu\n", hmap->capacity);
			printf("Hashmap size: %zu\n", hmap->set);
			printf("Hashmap load factor: %lu\n", hmap->load_factor);
			printf("\n");
		} else if (buff_cmp_raw(cmdline_buff, ":w", 2)) {
			if (read_line(&key_buff, "Enter key: ")) {
				printf("Error reading key\n");
				return 1;
			}
			printf("Key: %s\n", buff_borrow(key_buff));
			buff_return(key_buff);

			if (read_line(&cmdline_buff, "Enter value: ")) {
				printf("Error reading value\n");
				return 1;
			}
			hmap_set(hmap, buff_borrow(key_buff), buff_len(key_buff), buff_borrow(cmdline_buff), true);
		} else if (buff_cmp_raw(cmdline_buff, ":r", 2)) {
			if (read_line(&key_buff, "Enter key: ")) {
				printf("Error reading key\n");
				return 1;
			}
			printf("Key: %s\n", buff_borrow(key_buff));
			buff_return(key_buff);

			if (hmap_get(hmap, buff_borrow(key_buff), buff_len(key_buff), &cmdline_buff) == false) {
				printf("Key not found\n");
			} else {
				printf("Value: %s\n", buff_borrow(cmdline_buff));
			}
		} else if (buff_cmp_raw(cmdline_buff, ":d", 2)) {
			if (read_line(&key_buff, "Enter key: ")) {
				printf("Error reading key\n");
				return 1;
			}
			printf("Key: %s\n", buff_borrow(key_buff));
			buff_return(key_buff);
			

			if () == NULL) {
				printf("Key not found\n");
			} else {
				hmap_
				printf("Key deleted\n");
			}
		}

		if (read_line(&cmdline_buff, "Enter command: ")) {
			printf("Error reading command\n");
			return 1;
		}
	}

	return 0;
}

int read_line(neon_buff_t **cmdline_buff, char *prompt)
{
	char cmdline[1024];
	if (*cmdline_buff != NULL) {
		buff_force_free(*cmdline_buff);
	}
	printf("\n");
	printf("%s", prompt);
	scanf("%1023s", &cmdline);
	*cmdline_buff = buff_new(cmdline, strlen(cmdline));
	if (*cmdline_buff == NULL) {
		printf("Error creating command line buffer\n");
		return 1;
	} else if (buff_validate_utf8(cmdline_buff) == false) {
		printf("Error: command line is not valid UTF-8\n");
		return 1;
	}
	return 0;
}
