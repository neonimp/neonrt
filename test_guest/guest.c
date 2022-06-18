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
	uint8_t nonce[64] = {
		0xd5, 0xe6, 0x9d, 0xad, 0x42, 0x4d, 0x86, 0xbf, 0x7f, 0xc8, 0x9f, 0xe1, 0x18,
		0x64, 0x50, 0x94, 0x83, 0xb2, 0x12, 0xe3, 0x3a, 0xcb, 0x54, 0xf5, 0xd9, 0x28,
		0xe6, 0x70, 0xd, 0xe0, 0xf3, 0x5f, 0x3b, 0x9a, 0xf9, 0x5b, 0x17, 0xb0, 0x4,
		0x41, 0x42, 0x26, 0x97, 0xd, 0xba, 0x3d, 0x2, 0x59, 0xa4, 0xa2, 0xdb, 0x9a,
		0x37, 0xc0, 0x48, 0x44, 0x13, 0x74, 0xc8, 0x98, 0xfb, 0x4f, 0x9a, 0xf7
	};

	char lines[36][100] = {
		"1 Lorem ipsum dolor sit amet, consectetur adipiscing elit.",
		"2 Nulla mollis lacus nec ante ornare, sed finibus erat tempus.",
		"3 Ut condimentum felis eu dolor interdum, vitae suscipit orci bibendum.",
		"4 Fusce quis velit pellentesque mi tincidunt ornare.",
		"5 Aliquam placerat ante a diam mollis, molestie blandit lectus dignissim.",
		"6 Duis luctus sem quis odio dapibus pharetra.",
		"7 Fusce lacinia diam non velit dignissim, vel sagittis dui vestibulum.",
		"8 Aenean euismod quam non mauris mattis, ac posuere sem volutpat.",
		"9 Donec et diam at enim rhoncus iaculis.",
		"10 Sed faucibus ipsum eget justo tempor, ac iaculis ex ornare.",
		"11 Morbi ut erat commodo, gravida lacus aliquam, iaculis tortor.",
		"12 Sed semper magna sit amet imperdiet tincidunt.",
		"13 Phasellus at dui vel ligula consectetur tincidunt.",
		"14 Donec ac nisl eu enim viverra varius quis at lectus.",
		"15 Proin ac nunc sit amet quam malesuada egestas eu sit amet lectus.",
		"16 Morbi ut ante accumsan, imperdiet lacus vel, tristique diam.",
		"17 Pellentesque vitae risus ac turpis hendrerit tincidunt.",
		"18 Quisque quis nisl a ante consectetur facilisis.",
		"19 Praesent ut risus et metus interdum semper a vel mi.",
		"20 Phasellus vel est ut felis euismod dapibus.",
		"21 Vestibulum posuere massa quis eros commodo, sed condimentum justo consectetur.",
		"22 Nunc elementum lectus sit amet orci mollis tincidunt.",
		"23 Suspendisse ac quam eu sem elementum porttitor.",
		"24 Suspendisse lacinia nisl vel mauris venenatis condimentum.",
		"25 Vivamus tempus turpis vitae nisl vestibulum tincidunt.",
		"26 Aenean a neque tempor, imperdiet mi viverra, pulvinar eros.",
		"27 Nam iaculis risus non lectus sodales, vitae tincidunt erat pulvinar.",
		"28 Sed semper nulla consectetur, dapibus erat in, blandit justo.",
		"29 Duis condimentum lorem sit amet dapibus posuere.",
		"30 Nunc a lacus molestie, vestibulum lorem et, ultrices sem.",
		"31 Phasellus sed eros nec ipsum mollis suscipit vitae eget neque.",
		"32 Vestibulum feugiat elit eu urna consequat volutpat.",
		"33 Nulla iaculis lacus a nulla auctor, et pharetra massa eleifend.",
		"34 Pellentesque euismod lorem id posuere consectetur.",
		"35 Pellentesque ornare ex eget quam sodales, in cursus ipsum pretium.",
		"36 Curabitur quis nibh quis nunc vestibulum feugiat.",
	};

//	urandom = open("/dev/urandom", O_RDONLY);
//	read(urandom, nonce, 64);

	hmap = hashmap_new(40, nonce, 0, 2);

	ll = ll_new(NULL);
	for (size_t i = 0; i < 36; i++) {
		ll_append_node(ll,
		               rt_buff_new((uint8_t *)(lines + i), strnlen(lines[i], 100)));

		hashmap_set(hmap,
		            rt_buff_new((uint8_t *)(lines + i), strnlen(lines[i], 100)),
		            lines + ((i + 1) % 36)
		);
	}

	node = ll->first;
	while (node != NULL) {
		printf("%s\n", rt_buff_borrow(node->data));
		node = node->next;
	}

	printf("\nSome hashmap fun now\n\n");

	node = ll->first;
	char *tmp;
	while (node != NULL) {
		tmp = (char *)hashmap_get(hmap, node->data);
		printf("%s\n", tmp);
		node = node->next;
	}

	ll_free(ll);
	hashmap_free(hmap);

	return 0;
}
