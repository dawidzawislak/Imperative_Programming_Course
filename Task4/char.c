#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

// consider chars from [FIRST_CHAR, LAST_CHAR)
#define FIRST_CHAR 33
#define LAST_CHAR 127
#define MAX_CHARS (LAST_CHAR - FIRST_CHAR)
#define MAX_BIGRAMS ((LAST_CHAR - FIRST_CHAR) * (LAST_CHAR - FIRST_CHAR))

#define NEWLINE '\n'
#define IN_WORD 1

#define IN_LINE_COMMENT 1
#define IN_BLOCK_COMMENT 2

int count[MAX_BIGRAMS] = { 0 };

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp(const void* a, const void* b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	if (count[va] == count[vb]) return va - vb;	return count[vb] - count[va];
}

// sort indices according to their respective counts.
// sort alphabetically if counts equal
int cmp_di(const void* a, const void* b) {
	int va = *(int*)a;
	int vb = *(int*)b;
	// sort according to second char if counts and the first char equal
	if (count[va] == count[vb] && va / MAX_CHARS == vb / MAX_CHARS) return va % MAX_CHARS - vb % MAX_CHARS;
	// sort according to first char if counts equal
	if (count[va] == count[vb]) return va / MAX_CHARS - vb / MAX_CHARS;
	return count[vb] - count[va];
}

// count lines, words & chars in a given text file
void wc(int* nl, int* nw, int* nc) 
{
	*nl = *nw = *nc = 0;
	char ch = 0;
	int start = 0;
	while ((ch = fgetc(stdin)) != EOF)
	{
		(*nc)++;
		if (ch >= FIRST_CHAR && ch <= LAST_CHAR)
			start = 1;
		if (ch == ' ' || ch == '\t')
		{
			if (start == 1)
				(*nw)++;
			start = 0;
		}
		else if (ch == NEWLINE)
		{
			(*nl)++;
			if (start == 1)
				(*nw)++;
			start = 0;
		}
	}
}

typedef struct
{
	char ch;
	int cnt;
} dict;

int comp(const void* a, const void* b) {
	dict* x = (dict*)a;
	dict* y = (dict*)b;
	return y->cnt - x->cnt;
}

void char_count(int char_no, int* n_char, int* cnt) 
{
	dict counts[LAST_CHAR - FIRST_CHAR];
	for (int i = 0; i < LAST_CHAR - FIRST_CHAR; i++)
	{
		counts[i].cnt = 0;
		counts[i].ch = i+FIRST_CHAR;
	}

	char ch = 0;
	while ((ch = fgetc(stdin)) != EOF)
		if (ch >= FIRST_CHAR && ch < LAST_CHAR)
			counts[(int)ch-FIRST_CHAR].cnt++;

	qsort(counts, sizeof(counts) / sizeof(*counts), sizeof(*counts), comp);
	*n_char = counts[char_no-1].ch;
	*cnt = counts[char_no-1].cnt;
}

void bigram_count(int bigram_no, int bigram[])
{
	int c = 0;
	int diag = 0;
	int pow[MAX_CHARS * MAX_CHARS];
	int ind[MAX_CHARS * MAX_CHARS];
	int tmp = 0;
	int swapped = 0;

	for (int i = 0; i < MAX_CHARS * MAX_CHARS; i++)
	{
		pow[i] = 0;
		ind[i] = i;
	}

	while ((c = fgetc(stdin)) != EOF)
	{
		if (c - 33 >= 0 && c - 33 <= 93)
		{
			if (diag == 0)
				diag = 94 * (c - 33);
			else
			{
				diag += (c - 33);
				pow[diag]++;
				diag = 94 * (c - 33);
			}
		}

		else
			diag = 0;
	}

	for (int i = 0; i < MAX_CHARS * MAX_CHARS - 1; i++)
	{
		swapped = 0;
		for (int j = 0; j < MAX_CHARS * MAX_CHARS - 1 - i; j++)
		{
			if (pow[ind[j]] < pow[ind[j + 1]])
			{
				tmp = ind[j];
				ind[j] = ind[j + 1];
				ind[j + 1] = tmp;
				swapped = 1;
			}
		}

		if (swapped == 0)
			break;
	}

	bigram[0] = ind[bigram_no - 1] / 94 + 33;
	bigram[1] = ind[bigram_no - 1] % 94 + 33;
	bigram[2] = pow[(bigram[0] - 33) * 94 + (bigram[1] - 33)];
}

void find_comments(int* line_comment_counter, int* block_comment_counter) 
{
	int ch1 = 0;
	int ch2 = 0;
	int blockFlag = 0;
	int lineFlag = 0;
	int gap = 2;
	*line_comment_counter = 0;
	*block_comment_counter = 0;

	ch1 = fgetc(stdin);

	while ((ch2 = fgetc(stdin)) != EOF)
	{
		if (gap < 2)
			gap += 1;

		if (blockFlag == 0 && lineFlag == 0)
		{
			if (ch1 == '/')
			{
				if (ch2 == '/' && gap > 1)
				{
					*line_comment_counter += 1;
					gap = 0;
					lineFlag = 1;
				}
				else if (ch2 == '*')
				{
					blockFlag = 1;
					gap = 0;
				}
			}
		}
		if (ch1 == '*' && ch2 == '/' && blockFlag == 1 && gap == 2)
		{
			blockFlag = 0;
			*block_comment_counter += 1;
			gap = 0;
		}
		else if (ch2 == '\n')
			lineFlag = 0;
		ch1 = ch2;
	}
}

#define MAX_LINE 128

int read_int() {
	char line[MAX_LINE];
	fgets(line, MAX_LINE, stdin); // to get the whole line
	return (int)strtol(line, NULL, 10);
}

int main(void) {
	int to_do;
	int nl, nw, nc, char_no, n_char, cnt;
	int line_comment_counter, block_comment_counter;
	int bigram[3];

	to_do = read_int();
	switch (to_do) {
	case 1: // wc()
		wc(&nl, &nw, &nc);
		printf("%d %d %d\n", nl, nw, nc);
		break;
	case 2: // char_count()
		char_no = read_int();
		char_count(char_no, &n_char, &cnt);
		printf("%c %d\n", n_char, cnt);
		break;
	case 3: // bigram_count()
		char_no = read_int();
		bigram_count(char_no, bigram);
		printf("%c%c %d\n", bigram[0], bigram[1], bigram[2]);
		break;
	case 4:
		find_comments(&line_comment_counter, &block_comment_counter);
		printf("%d %d\n", block_comment_counter, line_comment_counter);
		break;
	default:
		printf("NOTHING TO DO FOR %d\n", to_do);
		break;
	}
	return 0;
}
