#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#define CHAR_TO_PTR(c) (void *)(long) (c)
#define PTR_TO_CHAR(p) (char)(long) (p)

struct stack {
	struct stack *next;
	void *data;
};

static struct stack *stack_push(struct stack *stack, void *data)
{
	struct stack *head;

	head = malloc(sizeof(*head));
	if (head == NULL)
		return stack;

	head->next = stack;
	head->data = data;
	return head;
}

static struct stack *stack_pop(struct stack *stack, void **data)
{
	struct stack *head;

	if (stack == NULL)
		return NULL;

	if (data != NULL)
		*data = stack->data;

	head = stack->next;
	free(stack);
	return head;
}

static bool stack_is_empty(const struct stack *stack)
{
	return stack == NULL;
}

static void test_push_a_pop_a(void)
{
	struct stack *s = NULL;
	void *data = NULL;

	s = stack_push(s, CHAR_TO_PTR('a'));
	assert(!stack_is_empty(s));

	s = stack_pop(s, &data);
	assert(stack_is_empty(s));
	assert(PTR_TO_CHAR(data) == 'a');
}

static void test_push_a_b_pop_b_a(void)
{
	struct stack *s = NULL;
	void *data = NULL;

	s = stack_push(s, CHAR_TO_PTR('a'));
	assert(!stack_is_empty(s));

	s = stack_push(s, CHAR_TO_PTR('b'));
	assert(!stack_is_empty(s));

	s = stack_pop(s, &data);
	assert(!stack_is_empty(s));
	assert(PTR_TO_CHAR(data) == 'b');

	s = stack_pop(s, &data);
	assert(stack_is_empty(s));
	assert(PTR_TO_CHAR(data) == 'a');
}

static void test_pop_from_empty(void)
{
	struct stack *s = NULL;
	void *data = CHAR_TO_PTR('x');

	assert(stack_is_empty(s));

	s = stack_pop(s, &data);
	assert(stack_is_empty(s));
	assert(PTR_TO_CHAR(data) == 'x');
}

static void test_pop_with_no_data(void)
{
	struct stack *s = NULL;

	s = stack_push(s, CHAR_TO_PTR('a'));
	assert(!stack_is_empty(s));

	s = stack_pop(s, NULL);
	assert(stack_is_empty(s));
}

int main(void)
{
	test_push_a_pop_a();
	test_push_a_b_pop_b_a();
	test_pop_from_empty();
	test_pop_with_no_data();

	return 0;
}
