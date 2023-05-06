#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int rand_from_interval(int a, int b) 
{
	return a + rand() % (b + 1 - a);
}

void shuffle(int n, int array[])
{
	for (int i = 0; i < n; i++)
		array[i] = i;

	for (int i = 0; i < n - 1; i++)
	{
		int k = rand_from_interval(i, n - 1);
		int temp = array[i];
		array[i] = array[k];
		array[k] = temp;
	}
}

void distribute_cards(int* a1, int* a2, int* deck)
{
	for (int i = 0; i < 26; i++)
	{
		a1[i] = deck[i];
		a2[i] = deck[i + 26];
	}
}

int insert_cards(int* player_cards, int ptr, int* len, int* to_insert, int how_many)
{
	int start = (ptr + *len) % 52;
	for (int i = start; i < start + how_many; i++)
		player_cards[i % 52] = to_insert[i - start];
	
	*len += how_many;
	return 0;
}

int pop_card(int* player_cards, int* ptr, int* len)
{
	if (*len > 0)
	{
		int to_pop = player_cards[*ptr];
		(*ptr) = ((*ptr)+1)%52;
		(*len)--;
		return to_pop;
	}

	return -1;
}

int card_val(int c)
{
	return c / (int)4;
}

void print_hand(int* cards, int ptr, int len)
{
	for (int i = ptr; i < ptr+len; i++)
		printf("%i ", cards[i%52]);
}


int main(void) 
{
	int seed, mode, maxc;
	scanf("%i%i%i", &seed, &mode, &maxc);

	srand(seed);

	int deck[52];
	shuffle(52, deck);

	int cards_p1[52];
	int p1_ptr = 0, p1_len = 26;

	int cards_p2[52];
	int p2_ptr = 0, p2_len = 26;

	distribute_cards(cards_p1, cards_p2, deck);

	int conflicts_counter = 0;

	while (conflicts_counter <= maxc)
	{
		int p1 = pop_card(cards_p1, &p1_ptr, &p1_len);
		int p2 = pop_card(cards_p2, &p2_ptr, &p2_len);
		// player 1 put stronger card
		if (card_val(p1) > card_val(p2))
		{
			int t[] = { p1, p2 };
			insert_cards(cards_p1, p1_ptr, &p1_len, t, 2);
			conflicts_counter++;
		}
		// player 2 put stronger card
		else if (card_val(p2) > card_val(p1))
		{
			int t[] = { p2, p1 };
			insert_cards(cards_p2, p2_ptr, &p2_len, t, 2);
			conflicts_counter++;
		}
		//war
		else if (mode == 1)
		{
			int t[] = { p1 };
			insert_cards(cards_p1, p1_ptr, &p1_len, t, 1);
			t[0] = p2;
			insert_cards(cards_p2, p2_ptr, &p2_len, t, 1);
			conflicts_counter++;
		}
		else if (mode == 0)
		{
			conflicts_counter++;
			int t1[52];
			int t2[52];
			for (int i = 0; i < 52; i++)
			{
				t1[i] = t2[i] = -2;
			}
			
			t1[0] = p1;
			t2[0] = p2;

			int ind = 1;
			int war = 1;

			while (war > 0)
			{
				for (int i = 0; i < 2; i++)
				{
					t1[ind] = pop_card(cards_p1, &p1_ptr, &p1_len);
					t2[ind] = pop_card(cards_p2, &p2_ptr, &p2_len);
					ind++;
				}
				if (t1[ind - 1] == -1 || t2[ind - 1] == -1)
				{
					int p1_used = 0, p2_used = 0;
					for (int i = 0; i < ind; i++)
					{
						if (t1[i] >= 0) p1_used++;
						if (t2[i] >= 0) p2_used++;
					}
					printf("1 %i %i", p1_len + p1_used, p2_len + p2_used);
					return 0;
				}
				else
					conflicts_counter++;
				if (card_val(t1[ind-1]) > card_val(t2[ind-1]))
				{
					insert_cards(cards_p1, p1_ptr, &p1_len, t1, ind);
					insert_cards(cards_p1, p1_ptr, &p1_len, t2, ind);
					war = 0;
				}
				else if (card_val(t1[ind-1]) < card_val(t2[ind-1]))
				{
					insert_cards(cards_p2, p2_ptr, &p2_len, t2, ind);
					insert_cards(cards_p2, p2_ptr, &p2_len, t1, ind);
					war = 0;
				}
				if (conflicts_counter > maxc && war > 0)
				{
					int p1_used = 0, p2_used = 0;
					for (int i = 0; i < ind; i++)
					{
						if (t1[i] >= 0) p1_used++;
						if (t2[i] >= 0) p2_used++;
					}
					printf("0 %i %i", p1_len + p1_used, p2_len + p2_used);
					return 0;
				}
			}
		}
		if (p1_len == 52)
		{
			printf("2 %i", conflicts_counter);
			return 0;
		}
		else if (p2_len == 52)
		{
			printf("3\n");
			print_hand(cards_p2, p2_ptr, p2_len);
			return 0;
		}
	}

	printf("0 %i %i", p1_len, p2_len);

	return 0;
}
