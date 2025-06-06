#include <cs50.h>
#include <stdio.h>
#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX] = {0};

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];
    
// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);
bool check_cycle(int head, int tail);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] voter's preference of ith candidate
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // iterating through each candidate name
    for (int i = 0; i < candidate_count; i++)
    {
        if (strcmp(candidates[i], name) == 0)
        {
            // candidates assigned with respecive rank
            ranks[rank] = i;
            return true;
        }
    }
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i + 1; j < candidate_count; j++)
        {
            // we are iterating through ranks list ranks[i] is winning over
            preferences[ranks[i]][ranks[j]]++;
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = i+1; j < candidate_count; j++)
        {
            if (preferences[i][j] > preferences[j][i])
            {
                // add pairs; pairs_count was initialized as 0
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
            else if (preferences[i][j] < preferences[j][i])
            {
                // add pairs
                pairs[pair_count].winner = j;
                pairs[pair_count].loser = i;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    // temporary variables declaration; sorting using bubble sort
    pair temp;
    int max_index;

    for (int i = 0; i < pair_count; i++)
    {
        max_index = i; // setting max index as the index of current first pair
        int max = 0;
        for (int j = i; j < pair_count; j++) // j = i, so sorted elements won't be affected
        {
            // checking the strength of jth pairs winning
            if (preferences[pairs[j].winner][pairs[j].loser] > max)
            {
                max = preferences[pairs[j].winner][pairs[j].loser];
                max_index = j;
            }
        }
        // swapping the current pair with max pair
        temp = pairs[i];
        pairs[i] = pairs[max_index];
        pairs[max_index] = temp;
    }
    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    //
    for (int i = 0; i < pair_count; i++)
    {
        //
        int head = pairs[i].winner;
        if (!check_cycle(head, pairs[i].loser))
        {
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }
    return;
}

// check the cycle dependancy
bool check_cycle(int head, int tail)
{
    // checking the possibility of cycle
    if (head == tail)
    {
        return true;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        // tail becomes the new temp head and follow the process
        if (locked[tail][i] == true && check_cycle(head, i))
        {
            // one more element found on graph
            return true;
        }
    }
    return false;
}

// Print the winner of the election
void print_winner(void)
{
    // TODO
    for (int i = 0; i < candidate_count; i++)
    {
        bool decision = true;
        for (int j = 0; j < candidate_count; j++)
        {
            if (locked[j][i])
            {
                decision = false;
            }
        }
        if (decision)
        {
            printf("%s\n", candidates[i]);
        }
    }
    return;
}
