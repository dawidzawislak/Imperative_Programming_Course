#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REL_SIZE 100
#define MAX_RANGE 100

typedef struct {
	int first;
	int second;
} pair;

// Add pair to existing relation if not already there
int add_relation(pair*, int, pair);

// Case 1:

// The relation R is reflexive if xRx for every x in X
int is_reflexive(const pair* relation, int size) {
	for (int j = 0; j < size; j++)
	{
		pair x_pair = { relation[j].first, relation[j].first };
		pair y_pair = { relation[j].second, relation[j].second };

		int flag = 0;
		for (int i = 0; i < size; i++) {
			if (cmp_pair(&relation[i], &x_pair) == 0) {
				flag += 1;
				break;
			}
		}
		for (int i = 0; i < size; i++) {
			if (cmp_pair(&relation[i], &y_pair) == 0) {
				flag += 1;
				break;
			}
		}
		if (flag < 2) return 0;
	}
	return 1;
}

// The relation R on the set X is called irreflexive
// if xRx is false for every x in X
int is_irreflexive(const pair* relation, int size) 
{
	for (int j = 0; j < size; j++)
	{
		pair x_pair = { relation[j].first, relation[j].first };
		pair y_pair = { relation[j].second, relation[j].second };

		int flag = 0;
		for (int i = 0; i < size; i++) {
			if (cmp_pair(&relation[i], &x_pair) == 0) {
				flag += 1;
				break;
			}
		}
		for (int i = 0; i < size; i++) {
			if (cmp_pair(&relation[i], &y_pair) == 0) {
				flag += 1;
				break;
			}
		}
		if (flag == 2) return 0;
	}
	return 1;
}

// A binary relation R over a set X is symmetric if:
// for all x, y in X xRy <=> yRx
int is_symmetric(const pair* relation, int size)
{
	for (int j = 0; j < size; j++)
	{
		pair yRx = { relation[j].second, relation[j].first };
		int flag = 0;
		for (int i = 0; i < size; i++) {
			if (cmp_pair(&relation[i], &yRx) == 0) {
				flag += 1;
				break;
			}
		}
		if (flag == 0)
			return 0;
	}
	return 1;
}

// A binary relation R over a set X is antisymmetric if:
// for all x,y in X if xRy and yRx then x=y
int is_antisymmetric(const pair* relation, int size)
{
	for (int j = 0; j < size; j++)
	{
		pair yRx = { relation[j].second, relation[j].first };
		int flag = 0;
		for (int i = 0; i < size; i++) {
			if (cmp_pair(&relation[i], &yRx) == 0) {
				flag += 1;
				break;
			}
		}
		if (flag == 1 && yRx.first != yRx.second)
			return 0;
	}
	return 1;
}

// A binary relation R over a set X is asymmetric if:
// for all x,y in X if at least one of xRy and yRx is false
int is_asymmetric(const pair* relation, int size)
{
	int flag = 0;
	for (int j = 0; j < size; j++)
	{
		pair yRx = { relation[j].second, relation[j].first };
		for (int i = 0; i < size; i++) {
			if (cmp_pair(&relation[i], &yRx) == 0) {
				flag += 1;
				break;
			}
		}
	}
	return flag == 0;
}

// A homogeneous relation R on the set X is a transitive relation if:
// for all x, y, z in X, if xRy and yRz, then xRz
int is_transitive(const pair* relation, int size)
{
	for (int j = 0; j < size; j++)
	{
		pair xRy = relation[j];
		for (int i = 0; i < size; i++) {
			pair yRz_candidate = relation[i];
			int flag = 0;
			if (xRy.second == yRz_candidate.first) {
				for (int k = 0; k < size; k++) {
					pair to_find = { xRy.first, yRz_candidate.second };
					if (cmp_pair(&relation[k], &to_find) == 0) {
						flag = 1;
						break;
					}
				}
				if (flag == 0) return 0;
			}
		}
	}
	return 1;
}

// Case 2:

// A partial order relation is a homogeneous relation that is
// reflexive, transitive, and antisymmetric
int is_partial_order(const pair* relation, int size)
{
	return (is_reflexive(relation, size)+is_transitive(relation, size)+is_antisymmetric(relation, size) == 3) ? 1 : 0;
}

// A total order relation is a partial order relation that is connected
int is_total_order(const pair* relation, int size)
{
	 return is_partial_order(relation, size)+is_connected(relation, size) == 2 ? 1 : 0;
}

// Relation R is connected if for each x, y in X:
// xRy or yRx (or both)
int is_connected(const pair* relation, int size)
{
	for (int j = 0; j < size; j++)
	{
		int x1 = relation[j].first;
		int x2 = relation[j].second;
		int f1 = 0, f2 = 0;
		for (int i = 0; i < size; i++) {
			int y1 = relation[i].first;
			int y2 = relation[i].second;

			for (int k = 0; k < size; k++) {
				pair to_cmp = { x1,y1 };
				if (cmp_pair(&relation[k], &to_cmp) != 0) return 0;
				to_cmp.second = y2;
				if (cmp_pair(&relation[k], &to_cmp) != 0) return 0;
				to_cmp.first = x2;
				if (cmp_pair(&relation[k], &to_cmp) != 0) return 0;
				to_cmp.second = y1;
				if (cmp_pair(&relation[k], &to_cmp) != 0) return 0;
			}
		}
	}
	return 1;
}

void my_sort(int* tab, int n)
{
	int i, key, j;
	for (i = 1; i < n; i++) {
		key = tab[i];
		j = i - 1;

		while (j >= 0 && tab[j] > key) {
			tab[j + 1] = tab[j];
			j = j - 1;
		}
		tab[j + 1] = key;
	}
}

int find_max_elements(const pair* relation, int size, int* max_elements) 
{
	int cnt = 0;
	for (int i = 0; i < size; i++)
	{
		int x = relation[i].first;
		int y = relation[i].second;
		int flagx = 1;
		int flagy = 1;
		for (int j = 0; j < size; j++)
		{
			if (i == j) continue;
			if (relation[j].first == x) flagx = 0;
			if (relation[j].first == y) flagy = 0;
		}
		if (flagx == 1 && x != y) max_elements[cnt++] = x;
		if (flagy == 1) max_elements[cnt++] = y;
	}
	my_sort(max_elements, cnt);
	return cnt;
}

int find_min_elements(const pair* relation, int size, int* min_elements)
{
	int cnt = 0;
	for (int i = 0; i < size; i++)
	{
		int x = relation[i].first;
		int y = relation[i].second;
		int flagx = 1;
		int flagy = 1;
		for (int j = 0; j < size; j++)
		{
			if (i == j) continue;
			if (relation[j].second == x) flagx = 0;
			if (relation[j].second == y) flagy = 0;
		}
		if (flagx == 1 && x != y) min_elements[cnt++] = x;
		if (flagy == 1) min_elements[cnt++] = y;
	}
	my_sort(min_elements, cnt);
	return cnt;
}



int get_domain(const pair* relation, int size, int* x)
{
	int* rel1 = (int*)malloc(size*sizeof(int));
	int* rel2 = (int*)malloc(size*sizeof(int));
	for (int i = 0; i < size; i++) {
		rel1[i] = relation[i].first;
		rel2[i] = relation[i].second;
	}

	my_sort(rel1, size);
	my_sort(rel2, size);
	int index = 0;
	int rel1p = 0, rel2p = 0;
	if (rel1[0] <= rel2[0]) {
		x[0] = rel1[0];
		rel1p++;
	}
	else {
		x[0] = rel2[0];
		rel2p++;
	}
	
	while (rel1p < size && rel2p < size)
	{
		if (rel1[rel1p] < rel2[rel2p]) {
			if (rel1[rel1p] > x[index]) {
				index += 1;
				x[index] = rel1[rel1p];
			}
			rel1p++;
		}
		if (rel1[rel1p] > rel2[rel2p]) {
			if (rel2[rel2p] > x[index]) {
				index += 1;
				x[index] = rel1[rel2p];
			}
			rel2p++;
		}
		if (rel1[rel1p] == rel2[rel2p]) {
			if (rel2[rel2p] > x[index]) {
				index += 1;
				x[index] = rel2[rel2p];
			}
			rel2p++;
			rel1p++;
		}
	}
	if (rel1p < size) {
		for (int i = rel1p; i < size; i++)
		{
			if (rel1[i] > x[index]) {
				index += 1;
				x[index] = rel1[i];
			}
		}
	}
	else if (rel2p < size) {
		for (int i = rel2p; i < size; i++)
		{
			if (rel2[i] > x[index]) {
				index += 1;
				x[index] = rel2[i];
			}
		}
	}

	free(rel1);
	free(rel2);
	return index + 1;
}

// Case 3:

int composition(const pair* r1, int s1, const pair* r2, int s2, pair* r3) 
{
	int cnt = 0;
	for (int i = 0; i < s1; i++)
	{
		for (int j = 0; j < s2; j++)
		{
			int flag = 0;
			if (r1[i].second == r2[j].first) {
				pair t = { r1[i].first, r2[j].second };
				int flag = 0;
				for (int k = 0; k < cnt; k++) {
					if (cmp_pair(&t, &r3[k]) == 0)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 0)
					r3[cnt++] = t;
			}
		}
	}
	return cnt;
}

// Comparator for pair
int cmp_pair(const void* a, const void* b) 
{
	pair* a_ = (pair*)a;
	pair* b_ = (pair*)b;

	if (a_->first == b_->first)
		return a_->second - b_->second;

	return a_->first - b_->first;
}

int insert_int(int* tab, int n, int new_element) {
}

// Add pair to existing relation if not already there
int add_relation(pair* tab, int n, pair new_pair) 
{
	for (int i = 0; i < n; i++)
		if (cmp_pair(&tab[i], &new_pair) == 0)
			return n;

	tab[n] = new_pair;

	return n+1;
}

// Read number of pairs, n, and then n pairs of ints
int read_relation(pair* relation) 
{
	int n;
	pair xy;
	int size = 0;
	scanf("%i", &n);

	for (int i = 0; i < n; i++) {
		scanf("%i %i", &xy.first, &xy.second);
		size = add_relation(relation, size, xy);
	}

	return n;
}

void print_int_array(const int* array, int n) 
{
	printf("%i\n", n);
	for (int i = 0; i < n; i++)
		printf("%i ", array[i]);
	printf("\n");
}

int main(void) {
	int to_do;
	pair relation[MAX_REL_SIZE];
	pair relation_2[MAX_REL_SIZE];
	pair comp_relation[MAX_REL_SIZE];
	int domain[MAX_REL_SIZE];
	int max_elements[MAX_REL_SIZE];
	int min_elements[MAX_REL_SIZE];

	scanf("%d", &to_do);
	int size = read_relation(relation);
	int ordered, size_2, n_domain;

	switch (to_do) {
	case 1:
		printf("%d ", is_reflexive(relation, size));
		printf("%d ", is_irreflexive(relation, size));
		printf("%d ", is_symmetric(relation, size));
		printf("%d ", is_antisymmetric(relation, size));
		printf("%d ", is_asymmetric(relation, size));
		printf("%d\n", is_transitive(relation, size));
		break;
	case 2:
		ordered = is_partial_order(relation, size);
		n_domain = get_domain(relation, size, domain);
		printf("%d %d\n", ordered, is_total_order(relation, size));
		print_int_array(domain, n_domain);
		if (!ordered) break;
		int no_max_elements = find_max_elements(relation, size, max_elements);
		int no_min_elements = find_min_elements(relation, size, min_elements);
		print_int_array(max_elements, no_max_elements);
		print_int_array(min_elements, no_min_elements);
		break;
	case 3:
		size_2 = read_relation(relation_2);
		printf("%d\n", composition(relation, size, relation_2, size_2, comp_relation));
		break;
	default:
		printf("NOTHING TO DO FOR %d\n", to_do);
		break;
	}
	return 0;
}
