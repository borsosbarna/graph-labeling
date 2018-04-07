#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cmath>
#include <algorithm>

using namespace std;

#define MIN_TEMPERATURE 0.00001

struct parameters
{
    int h;                      // min. difference between adjacent vertexes
    int k;                      // min. difference between vertexes at distance of 2 edges
    char* file;                 // input file path

    int temperature;            // starting system temperature
    double cooling_factor;      // system cooling coefficient (0 < x < 1)
    int max_iterations;         // max. number of iterations
    int max_time;               // max. running time (seconds)
};

struct data
{
    int vertex_count;           // number of vertexes
    int edge_count;             // number of edges

    int max_chromatic_number;   // max. chromatic number
    double min_temperature;     // min. number considered non-zero

    bool** graph;               // graph adjacency matrix
    int** neighbors;            // array of adjacent vertexes for every vertex
    int** second_neighbors;     // array of vertexes at distance of 2 edges for each vertex
};

struct solution
{
    int* data;                  // a solution

    bool is_correct;            // solution correctness
    int conflicting_vertexes;   // solution conflicting vertexes count
    int chromatic_number;       // solution chromatic number
    double fitness;             // solution fitness
};

struct state
{
    double temperature;         // current temperature
    int iteration;              // current iteration

    solution current_solution;  // current solution
    solution next_solution;     // next proposed solution
    solution best_solution;     // global best solution
};

void set_parameters(char**, parameters&);
void initialize(parameters&, data&);
void neighbors(data&);
void second_neighbours(data&);
void max_chromatic_number(parameters&, data&);

void initialize(parameters&, data&, state&);
bool is_correct(parameters&, data&, solution&);
bool is_conflicting_vertex(parameters&, data&, solution&, int);
int chromatic_number(data&, solution&);
double fitness(parameters&, data&, solution&);

void generate_next_solution(parameters&,data&, state&);
void cooling(parameters&, state&);
void print_state(data&, state&);

int main(int argc, char** argv)
{
    parameters parameters;
    data data;
    state state;

    srand(time(NULL));

    set_parameters(argv, parameters);
    initialize(parameters, data);
    initialize(parameters, data, state);

    while(state.temperature > data.min_temperature &&
          state.iteration < parameters.max_iterations &&
          clock() / CLOCKS_PER_SEC < parameters.max_time)
    {
        generate_next_solution(parameters, data, state);

        /* update current solution if needed */
        if(state.next_solution.fitness > state.current_solution.fitness || (double) rand() / RAND_MAX <
           exp((state.next_solution.fitness - state.current_solution.fitness) / state.temperature))
        {
            for(int i = 0; i < data.vertex_count; ++i)
            {
                state.current_solution.data[i] = state.next_solution.data[i];
            }

            state.current_solution.is_correct = state.next_solution.is_correct;
            state.current_solution.conflicting_vertexes = state.next_solution.conflicting_vertexes;
            state.current_solution.chromatic_number = state.next_solution.chromatic_number;
            state.current_solution.fitness = state.next_solution.fitness;

            /* update global best solution if needed */
            if(state.current_solution.fitness > state.best_solution.fitness)
            {
                for(int i = 0; i < data.vertex_count; ++i)
                {
                    state.best_solution.data[i] = state.current_solution.data[i];
                }

                state.best_solution.is_correct = state.current_solution.is_correct;
                state.best_solution.conflicting_vertexes = state.current_solution.conflicting_vertexes;
                state.best_solution.chromatic_number = state.current_solution.chromatic_number;
                state.best_solution.fitness = state.current_solution.fitness;
            }
        }

        cooling(parameters, state);
    }

    print_state(data, state);

    return 0;
}

void set_parameters(char** argv, parameters& parameters)
{
    /* store command line arguments in appropriate structures */
    parameters.h = atoi(argv[1]);
    parameters.k = atoi(argv[2]);

    parameters.file = new char[strlen(argv[3]) + 1];
    strcpy(parameters.file, argv[3]);

    parameters.temperature = atoi(argv[4]);
    parameters.cooling_factor = atof(argv[5]);
    parameters.max_iterations = atoi(argv[6]);
    parameters.max_time = atoi(argv[7]);
}

void initialize(parameters& parameters, data& data)
{
    /* read data from file */
    fstream file(parameters.file, ios_base::in);

    file >> data.vertex_count;
    file >> data.edge_count;

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

    /* close file */
    file.close();

    /* set/calculate constant data */
    data.min_temperature = MIN_TEMPERATURE;
    max_chromatic_number(parameters, data);

    neighbors(data);
    second_neighbours(data);
}

void max_chromatic_number(parameters& parameters, data& data)
{
    /* calculate max. chromatic number based on given data */
    double max_difference = max(parameters.h, parameters.k);
    double complete_graph = (data.vertex_count * (data.vertex_count - 1)) / 2;

    data.max_chromatic_number = round(((data.vertex_count) * max_difference) * (data.edge_count / complete_graph));
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

void second_neighbours(data& data)
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

void initialize(parameters& parameters, data& data, state& state)
{
    state.temperature = parameters.temperature;
    state.iteration = 0;

    state.current_solution.data = new int[data.vertex_count];

    for(int i = 0; i < data.vertex_count; ++i)
    {
        state.current_solution.data[i] = rand() % data.max_chromatic_number;
    }

    state.current_solution.is_correct = is_correct(parameters, data, state.current_solution);
    state.current_solution.chromatic_number = chromatic_number(data, state.current_solution);
    state.current_solution.fitness = fitness(parameters, data, state.current_solution);

    /* duplicate initial results */
    state.next_solution = state.current_solution;
    state.best_solution = state.current_solution;

    state.next_solution.data = new int[data.vertex_count];
    state.best_solution.data = new int[data.vertex_count];

    for(int i = 0; i < data.vertex_count; ++i)
    {
        state.next_solution.data[i] = state.current_solution.data[i];
        state.best_solution.data[i] = state.current_solution.data[i];
    }
}

bool is_correct(parameters& parameters, data& data, solution& solution)
{
    solution.conflicting_vertexes = 0;

    for(int i = 0; i < data.vertex_count; ++i)
    {
        if(is_conflicting_vertex(parameters, data, solution, i))
        {
            ++solution.conflicting_vertexes;
        }
    }

    return solution.conflicting_vertexes ? false : true;
}

bool is_conflicting_vertex(parameters& parameters, data& data, solution& solution, int vertex)
{
    /* check neighbors */
    for(int i = 1; i <= data.neighbors[vertex][0]; ++i)
    {
        if(abs(solution.data[vertex] - solution.data[data.neighbors[vertex][i]]) < parameters.h)
        {
            return true;
        }
    }

    /* check second neighbors */
    for(int i = 1; i <= data.second_neighbors[vertex][0]; ++i)
    {
        if(abs(solution.data[vertex] - solution.data[data.second_neighbors[vertex][i]]) < parameters.k)
        {
            return true;
        }
    }

    return false;
}

int chromatic_number(data& data, solution& solution)
{
    int* frequency_vector = new int[data.max_chromatic_number]();

    for(int i = 0; i < data.vertex_count; ++i)
    {
        ++frequency_vector[solution.data[i]];
    }

    int chromatic_number = 0;

    for(int i = 0; i < data.max_chromatic_number; ++i)
    {
        if(frequency_vector[i])
        {
            ++chromatic_number;
        }
    }

    delete[] frequency_vector;

    return chromatic_number;
}

double fitness(parameters& parameters, data& data, solution& solution)
{
    return (9 * (1 - (double)solution.conflicting_vertexes / data.vertex_count) + (1 - (double)solution.chromatic_number / data.max_chromatic_number)) / 10;
}

void generate_next_solution(parameters& parameters, data& data, state& state)
{
    /* copy current solution to next solution */
    for(int i = 0; i < data.vertex_count; ++i)
    {
        state.next_solution.data[i] = state.current_solution.data[i];
    }
    state.next_solution.is_correct = state.current_solution.is_correct;
    state.next_solution.conflicting_vertexes = state.current_solution.conflicting_vertexes;
    state.next_solution.chromatic_number = state.current_solution.chromatic_number;
    state.next_solution.fitness = state.current_solution.fitness;

    /* change a random conflicting vertex to a random label */
    int random_vertex;
    if(state.next_solution.conflicting_vertexes)
    {
        random_vertex = rand() % state.next_solution.conflicting_vertexes;
    }
    else
    {
        random_vertex = rand() % data.vertex_count;
    }

    for(int i = 0; i < data.vertex_count; ++i)
    {
        if(is_conflicting_vertex(parameters, data, state.next_solution, i) && 0 == random_vertex--)
        {
            state.next_solution.data[i] = rand() % data.max_chromatic_number;
            break;
        }
    }

    state.next_solution.is_correct = is_correct(parameters, data, state.next_solution);
    state.next_solution.chromatic_number = chromatic_number(data, state.next_solution);
    state.next_solution.fitness = fitness(parameters, data, state.next_solution);
}

void cooling(parameters& parameters, state& state)
{
    ++state.iteration;
    state.temperature *= parameters.cooling_factor;
    if(state.temperature < MIN_TEMPERATURE)
    {
        state.temperature = 0;
    }
}

void print_state(data& data, state& state)
{
    cout << clock() / CLOCKS_PER_SEC << endl;
    cout << state.iteration << endl;
    cout << state.temperature << endl;

    for(int i = 0; i < data.vertex_count; ++i)
    {
        cout << state.best_solution.data[i] << ' ';
    }
    cout << endl;

    cout << state.best_solution.is_correct << endl;
    cout << state.best_solution.conflicting_vertexes << endl;
    cout << state.best_solution.chromatic_number << endl;
    cout << state.best_solution.fitness << endl;
}
