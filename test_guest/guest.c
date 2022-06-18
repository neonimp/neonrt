#include <stdio.h>
#include <rt_buffer/rt_buffer.h>
#include <containers/linked_list.h>
#include <containers/hashmap.h>
#include <fcntl.h>
#include <memory.h>
#include <unistd.h>

int main(void)
{
	ll_t *ll;
	ll_node_t *node;
	hmap_t *hmap;
	int urandom;
	uint8_t nonce[64];

	char lines[36][100] = {
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
		"Nulla mollis lacus nec ante ornare, sed finibus erat tempus.",
		"Ut condimentum felis eu dolor interdum, vitae suscipit orci bibendum.",
		"Fusce quis velit pellentesque mi tincidunt ornare.",
		"Aliquam placerat ante a diam mollis, molestie blandit lectus dignissim.",
		"Duis luctus sem quis odio dapibus pharetra.",
		"Fusce lacinia diam non velit dignissim, vel sagittis dui vestibulum.",
		"Aenean euismod quam non mauris mattis, ac posuere sem volutpat.",
		"Donec et diam at enim rhoncus iaculis.",
		"Sed faucibus ipsum eget justo tempor, ac iaculis ex ornare.",
		"Morbi ut erat commodo, gravida lacus aliquam, iaculis tortor.",
		"Sed semper magna sit amet imperdiet tincidunt.",
		"Phasellus at dui vel ligula consectetur tincidunt.",
		"Donec ac nisl eu enim viverra varius quis at lectus.",
		"Proin ac nunc sit amet quam malesuada egestas eu sit amet lectus.",
		"Morbi ut ante accumsan, imperdiet lacus vel, tristique diam.",
		"Pellentesque vitae risus ac turpis hendrerit tincidunt.",
		"Quisque quis nisl a ante consectetur facilisis.",
		"Praesent ut risus et metus interdum semper a vel mi.",
		"Phasellus vel est ut felis euismod dapibus.",
		"Vestibulum posuere massa quis eros commodo, sed condimentum justo consectetur.",
		"Nunc elementum lectus sit amet orci mollis tincidunt.",
		"Suspendisse ac quam eu sem elementum porttitor.",
		"Suspendisse lacinia nisl vel mauris venenatis condimentum.",
		"Vivamus tempus turpis vitae nisl vestibulum tincidunt.",
		"Aenean a neque tempor, imperdiet mi viverra, pulvinar eros.",
		"Nam iaculis risus non lectus sodales, vitae tincidunt erat pulvinar.",
		"Sed semper nulla consectetur, dapibus erat in, blandit justo.",
		"Duis condimentum lorem sit amet dapibus posuere.",
		"Nunc a lacus molestie, vestibulum lorem et, ultrices sem.",
		"Phasellus sed eros nec ipsum mollis suscipit vitae eget neque.",
		"Vestibulum feugiat elit eu urna consequat volutpat.",
		"Nulla iaculis lacus a nulla auctor, et pharetra massa eleifend.",
		"Pellentesque euismod lorem id posuere consectetur.",
		"Pellentesque ornare ex eget quam sodales, in cursus ipsum pretium.",
		"Curabitur quis nibh quis nunc vestibulum feugiat.",
	};

	urandom = open("/dev/urandom", O_RDONLY);
	read(urandom, nonce, 64);

	hmap = hashmap_new(40, nonce, 0, 2);

	ll = ll_new(NULL);
	for (size_t i = 0; i < 36; i++) {
		ll_append_node(ll,
		               rt_buff_new((uint8_t *)(lines + i), strnlen(lines[i], 100) - 1));

		hashmap_set(hmap,
		            rt_buff_new((uint8_t *)(lines + i), strnlen(lines[i], 100) - 1),
		            lines + ((i + 1) % 36)
		);
	}

	node = ll->first;
	while (node != NULL) {
		printf("%s\n", rt_buff_borrow(node->dt_ptr));
		node = node->next;
	}

	printf("\nSome hashmap fun now\n\n");

	node = ll->first;
	char *tmp;
	while (node != NULL) {
		tmp = (char *)hashmap_get(hmap, node->dt_ptr);
		printf("%s\n", tmp);
		node = node->next;
	}

	ll_free(ll);
	hashmap_free(hmap);

	return 0;
}
