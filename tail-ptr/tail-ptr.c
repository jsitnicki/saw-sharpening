#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include <setjmp.h>
#include <cmocka.h>

typedef struct Element {
	struct Element *next;
	void *data;
} Element;

Element *head;
Element *tail;

bool list_remove(Element *elem)
{
	Element *prev = NULL;
	Element *cur = head;

	while (cur) {
		if (cur == elem) {
			if (cur == head)
				head = cur->next;
			if (cur == tail)
				tail = prev;
			if (prev)
				prev->next = cur->next;

			free(elem);
			return true;
		}
		prev = cur;
		cur = cur->next;
	}

	return false;
}

bool list_insert_after(Element *elem, int data)
{
	Element *cur = NULL;
	Element *next = head;

	do {
		if (cur == elem) {
			Element *tmp = malloc(sizeof(Element));
			if (!tmp)
				return false;
			tmp->data = (void *)(long) data;
			tmp->next = next;

			if (cur)
				cur->next = tmp;
			if (next == head)
				head = tmp;
			if (next == NULL)
				tail = tmp;

			return true;
		}
		cur = next;
		if (cur)
			next = cur->next;
	} while (cur);

	return false;
}

/* Tests */

#define CHAR_TO_PTR(c) (void *)(long) (c)
#define PTR_TO_CHAR(c) (char)(long) (c)

#define MAX_LIST_LEN 3

Element *A;
Element *B;
Element *C;

Element not_on_list = { NULL, CHAR_TO_PTR('X') };
Element *X = &not_on_list;
Element *NIL = NULL;

void make_list(int len)
{
	static Element **elems[] = { &A, &B, &C };
	int i;

	assert_in_range(len, 0, MAX_LIST_LEN);

	if (len == 0) {
		head = tail = NULL;
		return;
	}

	for (i = 0; i < len; i++) {
		*elems[i] = malloc(sizeof(Element));
		(*elems[i])->data = CHAR_TO_PTR('A' + i);
	}

	for (i = 0; i < len-1; i++) {
		(*elems[i])->next = *elems[i+1];
	}
	(*elems[i])->next = NULL;
	
	head = *elems[0];
	tail = *elems[len-1];
}

void free_list(void)
{
	Element *cur, *next;

	cur = head;
	while (cur) {
		next = cur->next;
		free(cur);
		cur = next;
	}
}

char elem_to_char(Element *elem)
{
	return elem ? PTR_TO_CHAR(elem->data) : '\0';
}

#define BUF_LEN 16

const char *list_to_str(Element *head)
{
	static char buf[BUF_LEN+1];
	Element *itr = head;
	int i = 0;

	while (i < BUF_LEN && itr) {
		buf[i++] = elem_to_char(itr);
		itr = itr->next;
	}
	buf[i] = '\0';

	return buf;
}

typedef struct {
	const char head;
	const char tail;
	const char *list;
} ListState;

#define EMPTY_LIST_STATE {'\0', '\0', "" }

void assert_list_state(const ListState *state)
{
	assert_int_equal(state->head, elem_to_char(head));
	assert_int_equal(state->tail, elem_to_char(tail));
	assert_string_equal(state->list, list_to_str(head));
}

void test_make_list(void **unused)
{
	(void) unused;

	const ListState states[] = {
		EMPTY_LIST_STATE,
		{ 'A', 'A', "A" },
		{ 'A', 'B', "AB" },
		{ 'A', 'C', "ABC" },
	};
	int n = sizeof(states)/sizeof(states[0]);

	while (n--) {
		make_list(n);
		assert_list_state(&states[n]);
		free_list();
	}
}

struct RemoveTest {
	int list_len;
	Element **elem;
	bool exp_result;
	ListState exp_state;
};

void test_remove(void **unused)
{
	(void) unused;

	static const struct RemoveTest tests[] = {
		{ 0, &NIL, false, EMPTY_LIST_STATE },
		{ 0, &X,   false, EMPTY_LIST_STATE },

		{ 1, &NIL, false, { 'A', 'A', "A" } },
		{ 1, &X,   false, { 'A', 'A', "A" } },
		{ 1, &A,   true,  EMPTY_LIST_STATE },

		{ 2, &NIL, false, { 'A', 'B', "AB" } },
		{ 2, &X,   false, { 'A', 'B', "AB" } },
		{ 2, &A,   true,  { 'B', 'B', "B" } },
		{ 2, &B,   true,  { 'A', 'A', "A" } },

		{ 3, &NIL, false, { 'A', 'C', "ABC" } },
		{ 3, &X,   false, { 'A', 'C', "ABC" } },
		{ 3, &A,   true,  { 'B', 'C', "BC" } },
		{ 3, &B,   true,  { 'A', 'C', "AC" } },
		{ 3, &C,   true,  { 'A', 'B', "AB" } },
	};
	int n_tests = sizeof(tests)/sizeof(tests[0]);

	int i;

	for (i = 0; i < n_tests; i++) {
		const struct RemoveTest *t = &tests[i];
		bool result;

		make_list(t->list_len);

		result = list_remove(*t->elem);
		assert_int_equal(t->exp_result, result);
		assert_list_state(&t->exp_state);

		free_list();
	}
}

struct InsertTest {
	int list_len;
	Element **elem;
	int data;
	bool exp_result;
	ListState exp_state;
};

void test_insert_after(void **unused)
{
	(void) unused;

	static const struct InsertTest tests[] = {
		{ 0, &X,   '@', false, EMPTY_LIST_STATE },
		{ 0, &NIL, '@', true,  { '@', '@', "@" } },

		{ 1, &X,   '@', false, { 'A', 'A', "A" } },
		{ 1, &NIL, '@', true,  { '@', 'A', "@A" } },
		{ 1, &A,   '@', true,  { 'A', '@', "A@" } },

		{ 2, &X,   '@', false, { 'A', 'B', "AB" } },
		{ 2, &NIL, '@', true,  { '@', 'B', "@AB" } },
		{ 2, &A,   '@', true,  { 'A', 'B', "A@B" } },
		{ 2, &B,   '@', true,  { 'A', '@', "AB@" } },

		{ 3, &X,   '@', false, { 'A', 'C', "ABC" } },
		{ 3, &NIL, '@', true,  { '@', 'C', "@ABC" } },
		{ 3, &A,   '@', true,  { 'A', 'C', "A@BC" } },
		{ 3, &B,   '@', true,  { 'A', 'C', "AB@C" } },
		{ 3, &C,   '@', true,  { 'A', '@', "ABC@" } },
	};
	static const int n_tests = sizeof(tests)/sizeof(tests[0]);

	int i;

	for (i = 0; i < n_tests; i++) {
		const struct InsertTest *t = &tests[i];
		bool result;

		make_list(t->list_len);

		result = list_insert_after(*t->elem, t->data);
		assert_int_equal(t->exp_result, result);
		assert_list_state(&t->exp_state);

		free_list();
	}
	
}


int main(void)
{
	const UnitTest tests[] = {
		unit_test(test_make_list),
		unit_test(test_remove),
		unit_test(test_insert_after),
	};
	
	return run_tests(tests);
}
