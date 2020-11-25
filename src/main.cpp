/** Trabalho de PPA - K SAT
 * 
 *   Professor
 *       Guilherme Koslovski
 *   Aluno
 *      Peter Brendel
 *
 *   Compilação:
 *     $ make -> Versão final
 *     $ make debug -> Versão pra debug
 * 
 **/

#include <bits/stdc++.h>
#include <unistd.h>
#include <mpi.h>
#include <omp.h>

#include "config.hpp"
#include "clause.hpp"
#include "ga.hpp"

using namespace std::chrono;
using timer = system_clock;

int main(int argc, char* argv[]) {

    int rank, size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if(argc < 2){
        std::cout << "Usage: ./a.out PathToInputFile" << std::endl;
        exit(1);
    }

    size_t bitsInInt = sizeof(int)*8 - 1;

    std::ifstream is (argv[1], std::fstream::in);

    std::string temp;
    std::string fname(argv[1]);
    int k, literalCount, clauseCount;
    // Read all lines until configuration (cnf) line is reached
    while(is >> temp && temp != "cnf");
    // Read configuration variables
    is >> k >> literalCount >> clauseCount;

    if (rank == 0) {
        printf("%d-SAT\n%d Literals\n%d Clauses\n", k, literalCount, clauseCount);
    }
    
    if (is.peek() == '\n') {
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    Config * config = new Config(fname, literalCount, clauseCount, k, size, rank);
    byte * literals = (byte*) malloc(sizeof(byte) * literalCount); //new byte[literalCount]; // C++ malloc
    Clause * clauses = (Clause*) calloc(clauseCount, sizeof(*clauses)); // C malloc )
    #ifdef DEBUG
    std::cout << sizeof(Clause) << " * " << clauseCount << " = " << sizeof(*clauses) * clauseCount << std::endl;
    std::cout << "sizeof(clauses[0]) = " << sizeof(clauses[0]) << std::endl;
    #endif
    unsigned clauseIndex = 0;
    #ifdef DEBUG
    int byteAmt = 0;
    #endif
    // Itera sobre o arquivo de entrada pra ler cada clausula
    while (is.peek() != '%') {
        int literal, index=0;
        std::shared_ptr<byte[]> literalPointers(new byte[k]);
        std::shared_ptr<byte[]> literalOperator(new byte[k]);
        
        while (is.peek() != '\n') {
            is >> literal;
            if (!literal) break;

            literalPointers[index] = (byte)abs(literal)-1;
            // Salvamos o endereco daquele liteal
            literalOperator[index] = ((unsigned)literal) >> bitsInInt;
            /* Salvamos `Shift intBits pra direita` no mesmo indice.
             *      Se literal < 0, resulta em 1. Estamos negando o literal (a XOR 1 = ~a)
             *      Caso contrario, resulta em 0. Nao vamos negar o literal (a XOR 0 =  a)
             */
            index++;
        };
        #ifdef DEBUG
        std::cout << sizeof(clauses[clauseIndex]) << std::endl;
        #endif
        clauses[clauseIndex] = Clause(literalPointers, literalOperator, k);
        #ifdef DEBUG
        std::cout << sizeof(clauses[clauseIndex]) << std::endl;
        #endif
        clauseIndex++;
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    t_individual out;
    GeneticAlgorithm ga = GeneticAlgorithm(config, clauses, literals, ceil(10000.0/size), 100, 0.8, 0.05);
    if (rank == 0){
        std::ofstream os (config->getFileName() + "-" + "mpi.dat", std::fstream::app);
        double start = MPI_Wtime();
        out = ga.evolve();
        os << MPI_Wtime() - start << '\n';
    } else {
        out = ga.evolve();
    }
    // os << duration_cast<milliseconds> (high_resolution_clock::now() - begin).count() << '\n';

    // std::cout << (float)out.first / config->getClauses() << std::endl;
    // std::cout << "---" << std::endl;
    
    MPI_Finalize();
    return 0;
}
