#include <stdio.h>
#include <neon_info.h>
#include <containers/buffer.h>
#include <containers/linked_list.h>
#include <containers/hashmap.h>
#include <util/wire.h>
#include <string.h>

void hmap_error_callback(__attribute__((unused)) const hmap_t *hmap, const char *msg, uint32_t error_code)
{
	printf("\n%s\nCode:%d\n", msg, error_code);
}

void hmap_unhealthy(hmap_t *hmap, size_t current_load)
{
	printf("\nUnhealthy: %zu\n", current_load);
	hmap_resize(hmap, hmap->capacity * 2);
	printf("\nResized: %zu\n", hmap->capacity);
}

int main(void)
{
	linked_list_t *ll;
	ll_node_t *node;
	hmap_t *hmap;

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

	printf("\nRT %s Compiled with: %s\n", RT_VERSION_STRING, neon_get_compiler());

	hmap = hmap_new(100, 75, 0, NULL);
	hmap_set_error_callback(hmap, hmap_error_callback);
	hmap_set_unhealthy_callback(hmap, hmap_unhealthy);

	ll = ll_new(NULL);
	for (size_t i = 0; i < 36; i++) {
		ll_append_node(ll,
		               buff_new((uint8_t *)(lines + i), strnlen(lines[i], 100)));

		hmap_set(hmap, (uint8_t *)(lines + i), strnlen(lines[i], 100), lines + i, false);
		printf("%lu, %lu, %lu\n", hmap->set, hmap->load_factor, hmap->collisions);
	}

	printf("\nSome hashmap fun now\n\n");

	node = ll->first;
	char *tmp;
	while (node != NULL) {
		printf("%s: ", buff_borrow(node->data));
		tmp = (char *)hmap_get(hmap, buff_borrow(node->data), buff_len(node->data));
		buff_return_n(node->data, 2);
		printf("%s\n", tmp);
		node = node->next;
	}

	ll_free(ll);
	hmap_free(hmap);

	return 0;
}
