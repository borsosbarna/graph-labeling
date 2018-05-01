/*  Borsos Barna

    INPUT
        argv[1]  - h (min. difference between neighbors)
        argv[2]  - k (min. difference between second order neighbors)
        argv[3]  - file
        argv[4]  - max. label value
        argv[5]  - populations count
        argv[6]  - population size
        argv[7]  - mutation chance
        argv[8]  - elites count
        argv[9]  - max. generations
        argv[10] - max. running time

    OUTPUT
        line 1: time elapsed
        line 2: generations created
        line 3: best fitness per generation
        line 4: solution found
        line 5: solution correctness
        line 6: number of conflicting vertexes
        line 7: chromatic number
        line 8: fitness
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <cmath>
#include <ctime>
#include <algorithm>

#define SELECTION_PERCENTAGE 0.05  // 0 <= x <= 1

using namespace std;

struct parameters
{
    int h;                          // min. difference between adjacent vertexes
    int k;                          // min. difference between vertexes at distance of 2 edges
    char* file;                     // input file path
    int max_label;                  // max. value of a label

    int populations_count;          // number of populations
    int population_size;            // number of chromosomes in one population
    double mutation_chance;         // chance of a chromosome to mutate (0 <= x <= 1)
    int elites;                     // number of fittest chromosomes to keep

    int max_generations;            // max. number of iterations
    int max_time;                   // max. running time (seconds)
};

struct data
{
    int vertex_count;               // number of vertexes
    int edge_count;                 // number of edges
    int fixed_vertex_count;         // number of predefined vertexes

    bool** graph;                   // graph adjacency matrix
    int** neighbors;                // array of adjacent vertexes for every vertex
    int** second_neighbors;         // array of vertexes at distance of 2 edges for each vertex
    int* solution_backbone;         // fixed vertexes of the solution
};

struct chromosome
{
    int* genes;                     // proposed solution

    bool is_correct;                // solution correctness
    int conflicting_genes;          // solution conflicting vertexes count
    int chromatic_number;           // solution chromatic number
    double fitness;                 // solution fitness
};

struct population
{
    chromosome* chromosomes;        // grouped solutions
};

struct generation
{
    int generation_number;          // iteration counter
    double* fitness_history;        // best fitness from every generation
    chromosome* best_chromosome;    // best solution

    population* populations;        // array of grouped solutions
};

void set_parameters(char**, parameters&);
void initialize(parameters&, data&);
void neighbors(data&);
void second_neighbors(data&);

void initialize(parameters&, data&, generation&, generation&);
bool is_correct(parameters&, data&, chromosome&);
bool is_conflicting_gene(parameters&, data&, chromosome&, int);
int chromatic_number(parameters&, data&, chromosome&);
double fitness(parameters&, data&, chromosome&);

void create_next_generation(parameters&, data&, generation&, generation&);
int selection(parameters&, population&);
void crossover(data&, chromosome&, chromosome&, chromosome&);
void mutation(parameters&, data&, chromosome&);
bool compareByFitness(const chromosome&, const chromosome&);
void print_data(parameters&, data&, generation&);

int main(int argc, char** argv)
{
    try
    {
        parameters parameters;
        data data;
        generation generation1;
        generation generation2;

        srand(time(NULL));

        set_parameters(argv, parameters);
        initialize(parameters, data);
        initialize(parameters, data, generation1, generation2);

        while(generation1.generation_number < parameters.max_generations &&
              clock() / CLOCKS_PER_SEC < parameters.max_time)
        {
            if(generation1.generation_number % 2)
            {
                create_next_generation(parameters, data, generation2, generation1);
            }
            else
            {
                create_next_generation(parameters, data, generation1, generation2);
            }
        }

        if(generation1.generation_number & 2)
        {
            print_data(parameters, data, generation2);
        }
        else
        {
            print_data(parameters, data, generation1);
        }
    }
    catch(...)
    {
        cout << "Error: check parameters!" << endl;
        return 1;
    }

    return 0;
}

void set_parameters(char** argv, parameters& parameters)
{
    /* store command line arguments in appropriate structures */
    parameters.h = atoi(argv[1]);
    parameters.k = atoi(argv[2]);

    parameters.file = new char[strlen(argv[3]) + 1];
    strcpy(parameters.file, argv[3]);

    parameters.max_label         = atoi(argv[4]);
    parameters.populations_count = atoi(argv[5]);
    parameters.population_size   = atoi(argv[6]);
    parameters.mutation_chance   = atof(argv[7]);
    parameters.elites            = atoi(argv[8]);
    parameters.max_generations   = atoi(argv[9]);
    parameters.max_time          = atoi(argv[10]);
}

void initialize(parameters& parameters, data& data)
{
    /* read data from file */
    fstream file(parameters.file, ios_base::in);

    file >> data.vertex_count;
    file >> data.edge_count;
    file >> data.fixed_vertex_count;

    /* allocate memory for adjacency matrix */
    data.graph = new bool*[data.vertex_count];
    for(int i = 0; i < data.vertex_count; ++i)
    {
        data.graph[i] = new bool[data.vertex_count]();
    }

    /* fill adjacency matrix */
    int vertex1, vertex2;
    for(int i = 0; i < data.edge_count; ++i)
    {
        file >> vertex1 >> vertex2;
        --vertex1; --vertex2;

        data.graph[vertex1][vertex2] = true;
        data.graph[vertex2][vertex1] = true;
    }

    /* create solution backbone */
    data.solution_backbone = new int[data.vertex_count]();
    int vertex, value;
    for (int i = 0; i < data.fixed_vertex_count; ++i)
    {
        file >> vertex >> value;
        data.solution_backbone[--vertex] = value;
    }

    /* close file */
    file.close();

    neighbors(data);
    second_neighbors(data);
}

void neighbors(data& data)
{
    /* allocate memory for every vertex */
    data.neighbors = new int*[data.vertex_count];

    /* for every vertex */
    for(int i = 0; i < data.vertex_count; ++i)
    {
        /* calculate neighbors count */
        int neighbors_count = 0;
        for(int j = 0; j < data.vertex_count; ++j)
        {
            if(data.graph[i][j])
            {
                ++neighbors_count;
            }
        }

        /* allocate memory for neighbors */
        data.neighbors[i] = new int[neighbors_count + 1];

        /* fill neighbors */
        int index = 0;
        data.neighbors[i][0] = neighbors_count;

        for(int j = 0; j < data.vertex_count; ++j)
        {
            if(data.graph[i][j])
            {
                data.neighbors[i][++index] = j;
            }
        }
    }
}

void second_neighbors(data& data)
{
    /* allocate memory for every vertex */
    data.second_neighbors = new int*[data.vertex_count];

    for(int i = 0; i < data.vertex_count; ++i)
    {
        /* calculate second neighbors count */
        int second_neighbors_count = 0;

        for(int j = 1; j <= data.neighbors[i][0]; ++j)
        {
            second_neighbors_count += data.neighbors[data.neighbors[i][j]][0];
        }

        /* allocate memory for initial second neighbors */
        int* second_neighbors = new int[second_neighbors_count + 1];

        /* fill second neighbors */
        int index = 0;
        second_neighbors[0] = second_neighbors_count;

        for(int j = 1; j <= data.neighbors[i][0]; ++j)
        {
            for(int k = 1; k <= data.neighbors[data.neighbors[i][j]][0]; ++k)
            {
                second_neighbors[++index] = data.neighbors[data.neighbors[i][j]][k];
            }
        }

        /* make second neighbors list a set */
        int final_count = 0;
        sort(second_neighbors + 1, second_neighbors + second_neighbors_count + 1);

        for(int j = 1; j <= second_neighbors_count; ++j)
        {
            if(second_neighbors[j] != i)
            {
                if(j == 1)
                {
                    ++final_count;
                }
                else if(second_neighbors[j] != second_neighbors[j - 1])
                {
                    ++final_count;
                }
            }
        }

        /* allocate memory for final second neighbors */
        data.second_neighbors[i] = new int[final_count + 1];

        index = 0;
        data.second_neighbors[i][0] = final_count;

        for(int j = 1; j <= second_neighbors_count; ++j)
        {
            if(second_neighbors[j] != i)
            {
                if(j == 1)
                {
                    data.second_neighbors[i][++index] = second_neighbors[j];
                }
                else if(second_neighbors[j] != second_neighbors[j - 1])
                {
                    data.second_neighbors[i][++index] = second_neighbors[j];
                }
            }
        }

        delete[] second_neighbors;
    }
}

void initialize(parameters& parameters, data& data, generation& g1, generation& g2)
{
    g1.fitness_history = g2.fitness_history = new double[parameters.max_generations]();
    g1.best_chromosome = g2.best_chromosome = new chromosome;
    g1.best_chromosome->genes = new int[data.vertex_count];
    g1.best_chromosome->fitness = -1;

    g1.generation_number = 0;
    g2.generation_number = 0;

    /* allocate memory for populations */
    g1.populations = new population[parameters.populations_count];
    g2.populations = new population[parameters.populations_count];

    /* allocate memory for chromosomes */
    for(int i = 0; i < parameters.populations_count; ++i)
    {
        g1.populations[i].chromosomes = new chromosome[parameters.population_size];
        g2.populations[i].chromosomes = new chromosome[parameters.population_size];

        /* allocate memory for genes */
        for(int j = 0; j < parameters.population_size; ++j)
        {
            g1.populations[i].chromosomes[j].genes = new int[data.vertex_count];
            g2.populations[i].chromosomes[j].genes = new int[data.vertex_count];

            /* initialize random genes */
            for(int k = 0; k < data.vertex_count; ++k)
            {
                if(data.solution_backbone[k])
                {
                    g1.populations[i].chromosomes[j].genes[k] = data.solution_backbone[k];
                }
                else
                {
                    g1.populations[i].chromosomes[j].genes[k] = rand() % parameters.max_label + 1;
                }
            }

            /* evaluate chromosome */
            g1.populations[i].chromosomes[j].is_correct = is_correct(parameters, data, g1.populations[i].chromosomes[j]);
            g1.populations[i].chromosomes[j].chromatic_number = chromatic_number(parameters, data, g1.populations[i].chromosomes[j]);
            g1.populations[i].chromosomes[j].fitness = fitness(parameters, data, g1.populations[i].chromosomes[j]);

            if(g1.populations[i].chromosomes[j].fitness > g1.fitness_history[0])
            {
                g1.fitness_history[0] = g1.populations[i].chromosomes[j].fitness;

                if(g1.populations[i].chromosomes[j].fitness > g1.best_chromosome->fitness)
                {
                    copy(g1.populations[i].chromosomes[j].genes,
                         g1.populations[i].chromosomes[j].genes + data.vertex_count,
                         g1.best_chromosome->genes);

                    g1.best_chromosome->is_correct = g1.populations[i].chromosomes[j].is_correct;
                    g1.best_chromosome->conflicting_genes = g1.populations[i].chromosomes[j].conflicting_genes;
                    g1.best_chromosome->chromatic_number = g1.populations[i].chromosomes[j].chromatic_number;
                    g1.best_chromosome->fitness = g1.populations[i].chromosomes[j].fitness;
                }
            }
        }
    }
}

bool is_correct(parameters& parameters, data& data, chromosome& chromosome)
{
    bool correctness = true;
    chromosome.conflicting_genes = 0;

    for(int i = 0; i < data.vertex_count; ++i)
    {
        if(is_conflicting_gene(parameters, data, chromosome, i))
        {
            if(data.solution_backbone[i] == 0)
            {
                ++chromosome.conflicting_genes;
            }
            correctness = false;
        }
    }

    return correctness;
}

bool is_conflicting_gene(parameters& parameters, data& data, chromosome& chromosome, int gene)
{
    /* check neighbors */
    for(int i = 1; i <= data.neighbors[gene][0]; ++i)
    {
        if(abs(chromosome.genes[gene] - chromosome.genes[data.neighbors[gene][i]]) < parameters.h)
        {
            return true;
        }
    }

    /* check second neighbors */
    for(int i = 1; i <= data.second_neighbors[gene][0]; ++i)
    {
        if(abs(chromosome.genes[gene] - chromosome.genes[data.second_neighbors[gene][i]]) < parameters.k)
        {
            return true;
        }
    }

    return false;
}

int chromatic_number(parameters& parameters, data& data, chromosome& chromosome)
{
    int max_label = chromosome.genes[0];

    for(int i = 1; i < data.vertex_count; ++i)
    {
        if(chromosome.genes[i] > max_label)
        {
            max_label = chromosome.genes[i];
        }
    }

    return max_label;
}

double fitness(parameters& parameters, data& data, chromosome& chromosome)
{
    return (4 * (1 - (double)chromosome.conflicting_genes / data.vertex_count) + (1 - (double)chromosome.chromatic_number / parameters.max_label)) / 5;
}

void create_next_generation(parameters& parameters, data& data, generation& current_generation, generation& next_generation)
{
    /* increment both iteration counters */
    next_generation.generation_number = ++current_generation.generation_number;

    /* for every population */
    for(int i = 0; i < parameters.populations_count; ++i)
    {
        /* save elites */
        sort(current_generation.populations[i].chromosomes, current_generation.populations[i].chromosomes + parameters.population_size, compareByFitness);

        for(int j = 0; j < parameters.elites; ++j)
        {
            copy(current_generation.populations[i].chromosomes[j].genes,
                 current_generation.populations[i].chromosomes[j].genes + data.vertex_count,
                 next_generation.populations[i].chromosomes[j].genes);

            next_generation.populations[i].chromosomes[j].is_correct = current_generation.populations[i].chromosomes[j].is_correct;
            next_generation.populations[i].chromosomes[j].conflicting_genes = current_generation.populations[i].chromosomes[j].conflicting_genes;
            next_generation.populations[i].chromosomes[j].chromatic_number = current_generation.populations[i].chromosomes[j].chromatic_number;
            next_generation.populations[i].chromosomes[j].fitness = current_generation.populations[i].chromosomes[j].fitness;

            if(next_generation.populations[i].chromosomes[j].fitness > next_generation.fitness_history[next_generation.generation_number])
            {
                next_generation.fitness_history[next_generation.generation_number] = next_generation.populations[i].chromosomes[j].fitness;

                if(next_generation.populations[i].chromosomes[j].fitness > next_generation.best_chromosome->fitness)
                {
                    copy(next_generation.populations[i].chromosomes[j].genes,
                         next_generation.populations[i].chromosomes[j].genes + data.vertex_count,
                         next_generation.best_chromosome->genes);

                    next_generation.best_chromosome->is_correct = next_generation.populations[i].chromosomes[j].is_correct;
                    next_generation.best_chromosome->conflicting_genes = next_generation.populations[i].chromosomes[j].conflicting_genes;
                    next_generation.best_chromosome->chromatic_number = next_generation.populations[i].chromosomes[j].chromatic_number;
                    next_generation.best_chromosome->fitness = next_generation.populations[i].chromosomes[j].fitness;
                }
            }
        }

        /* for every new chromosome */
        for(int j = parameters.elites; j < parameters.population_size; ++j)
        {
            int parent1_index = selection(parameters, current_generation.populations[i]);
            int parent2_index = selection(parameters, current_generation.populations[i]);

            crossover(data,
                      current_generation.populations[i].chromosomes[parent1_index],
                      current_generation.populations[i].chromosomes[parent2_index],
                      next_generation.populations[i].chromosomes[j]);

            mutation(parameters, data, next_generation.populations[i].chromosomes[j]);

            /* evaluate chromosome */
            next_generation.populations[i].chromosomes[j].is_correct = is_correct(parameters, data, next_generation.populations[i].chromosomes[j]);
            next_generation.populations[i].chromosomes[j].chromatic_number = chromatic_number(parameters, data, next_generation.populations[i].chromosomes[j]);
            next_generation.populations[i].chromosomes[j].fitness = fitness(parameters, data, next_generation.populations[i].chromosomes[j]);

            if(next_generation.populations[i].chromosomes[j].fitness > next_generation.fitness_history[next_generation.generation_number])
            {
                next_generation.fitness_history[next_generation.generation_number] = next_generation.populations[i].chromosomes[j].fitness;

                if(next_generation.populations[i].chromosomes[j].fitness > next_generation.best_chromosome->fitness)
                {
                    copy(next_generation.populations[i].chromosomes[j].genes,
                         next_generation.populations[i].chromosomes[j].genes + data.vertex_count,
                         next_generation.best_chromosome->genes);

                    next_generation.best_chromosome->is_correct = next_generation.populations[i].chromosomes[j].is_correct;
                    next_generation.best_chromosome->conflicting_genes = next_generation.populations[i].chromosomes[j].conflicting_genes;
                    next_generation.best_chromosome->chromatic_number = next_generation.populations[i].chromosomes[j].chromatic_number;
                    next_generation.best_chromosome->fitness = next_generation.populations[i].chromosomes[j].fitness;
                }
            }
        }
    }
}

int selection(parameters& parameters, population& population)
{
    /* initial random selection */
    int index = rand() % parameters.population_size;
    double fitness = population.chromosomes[index].fitness;

    /* calculate the max number of random candidates to pick for the tournament */
    int max_candidates = floor(parameters.population_size * SELECTION_PERCENTAGE) - 1;

    for(int i = 0; i < max_candidates; ++i)
    {
        int random_index = rand() % parameters.population_size;

        if(population.chromosomes[random_index].fitness > fitness)
        {
            index = random_index;
            fitness = population.chromosomes[random_index].fitness;
        }
    }

    return index;
}

void crossover(data& data, chromosome& parent1, chromosome& parent2, chromosome& offspring)
{
    double total_fitness = parent1.fitness + parent2.fitness;
    double parent1_dominance = parent1.fitness / total_fitness;

    for(int i = 0; i < data.vertex_count; ++i)
    {
        if((double)rand() / RAND_MAX < parent1_dominance)
        {
            offspring.genes[i] = parent1.genes[i];
        }
        else
        {
            offspring.genes[i] = parent2.genes[i];
        }
    }
}

void mutation(parameters& parameters, data& data, chromosome& chromosome)
{
    if((double)rand() / RAND_MAX > parameters.mutation_chance)
    {
        return;
    }

    /* change a random conflicting vertex to a random label */
    int random_gene;

    if(chromosome.conflicting_genes)
    {
        random_gene = rand() % chromosome.conflicting_genes;

        for(int i = 0; i < data.vertex_count; ++i)
        {
            if(0 == data.solution_backbone[i] &&
               is_conflicting_gene(parameters, data, chromosome, i) &&
               0 == random_gene--)
            {
                chromosome.genes[i] = rand() % parameters.max_label + 1;
                break;
            }
        }
    }
    else
    {
        random_gene = rand() % (data.vertex_count - data.fixed_vertex_count);

        for(int i = 0; i < data.vertex_count; ++i)
        {
            if(0 == data.solution_backbone[i] && 0 == random_gene--)
            {
                chromosome.genes[i] = rand() % parameters.max_label + 1;
                break;
            }
        }
    }
}

bool compareByFitness(const chromosome& a, const chromosome& b)
{
    return a.fitness > b.fitness;
}

void print_data(parameters& parameters, data& data, generation& generation)
{
    cout << (double) clock() / CLOCKS_PER_SEC << endl;
    cout << generation.generation_number << endl;

    /* for(int i = 0; i < parameters.populations_count; ++i)
    {
        for(int j = 0; j < parameters.population_size; ++j)
        {
            for(int k = 0; k < data.vertex_count; ++k)
            {
                cout << generation.populations[i].chromosomes[j].genes[k] << ' ';
            }

            cout << '\t';
            cout << generation.populations[i].chromosomes[j].is_correct << ' ';
            cout << generation.populations[i].chromosomes[j].conflicting_genes << ' ';
            cout << generation.populations[i].chromosomes[j].chromatic_number << ' ';
            cout << generation.populations[i].chromosomes[j].fitness << ' ';

            cout << endl;
        }
        cout << endl;
    } */

    for(int i = 0; i < data.vertex_count; ++i)
    {
        cout << generation.best_chromosome->genes[i] << ' ';
    }
    cout << endl;

    cout << generation.best_chromosome->is_correct << endl;
    cout << generation.best_chromosome->conflicting_genes << endl;
    cout << generation.best_chromosome->chromatic_number << endl;
    cout << generation.best_chromosome->fitness << endl;

    for(int i = 0; i < parameters.max_generations; i += ceil((double) parameters.max_generations / 100))
    {
        cout << generation.fitness_history[i] << ' ';
    }
}
