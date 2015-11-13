#include <assert.h>
#include <stdlib.h>

typedef struct Element {
	struct Element *next;
} Element;

Element *list_find_mth_to_last(Element *head, int m)
{
	Element *mth, *last;

	if (!head)
		return NULL;
	if (m < 0)
		return NULL;

	mth = last = head;

	while (m > 0 && last->next) {
		--m;
		last = last->next;
	}

	/* list too short */
	if (m > 0)
		return NULL;

	while (last->next) {
		mth = mth->next;
		last = last->next;
	}

	return mth;
}

/*
 * Tests
 */

void test_empty_list(void)
{
	Element *head = NULL;

	assert(NULL == list_find_mth_to_last(head, -1));
	assert(NULL == list_find_mth_to_last(head, 0));
	assert(NULL == list_find_mth_to_last(head, 1));
}

void test_one_element_list(void)
{
	Element elems[1] = { 
		{ .next = NULL },
	};
	Element *head = &elems[0];

	assert(NULL == list_find_mth_to_last(head, -1));
	assert(&elems[0] == list_find_mth_to_last(head, 0));
	assert(NULL == list_find_mth_to_last(head, 1));
}

void test_two_element_list(void)
{
	Element elems[2] = {
		{ .next = &elems[1] },
		{ .next = NULL },
	};
	Element *head = &elems[0];

	assert(NULL == list_find_mth_to_last(head, -1));
	assert(&elems[1] == list_find_mth_to_last(head, 0));
	assert(&elems[0] == list_find_mth_to_last(head, 1));
	assert(NULL == list_find_mth_to_last(head, 2));
}

int main(void)
{
	test_empty_list();
	test_one_element_list();
	test_two_element_list();

	return 0;
}
