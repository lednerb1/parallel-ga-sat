#pragma once
#include <bits/stdc++.h>
#include <omp.h>
#include <mpi.h>
#include "config.hpp"
#include "clause.hpp"

typedef std::pair<int, std::vector<byte>> t_individual;
typedef struct {
    int fitness;
    int * chromosome;
} s_individual;

std::ostream& operator<<(std::ostream& os, const std::vector<byte>& i) {
    os << "[";
    for (auto const& ii : i) {
        os << (int)ii << " ";
    }
    os << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, const t_individual& i) {
    os << std::endl << i.second << " Fitness: " << i.first;
    return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<t_individual>& i) {
    for (auto const& ii : i) {
        os << ii << " ";
    }
    return os;
}

class GeneticAlgorithm {
    Config * m_config;
    Clause * m_clauses;
    byte * m_literals;
    unsigned m_generations;
    float m_crossover;
    float m_mutation;
    unsigned m_populationSize;
   
    int GAMT = 5;
    public:
    GeneticAlgorithm(Config *& config, Clause*& clauses, byte *& literals, 
                    unsigned populationSize ,unsigned generations, float crossover, float mutation)
                    : m_config(config), m_clauses(clauses), m_literals(literals), 
                    m_populationSize(populationSize), m_generations(generations), m_crossover(crossover), m_mutation(mutation)
    {
        srand(time(NULL));        
    }

    int fitness(std::vector<byte>& chromosome) {
        int clauses = m_config->getClauses();
        int good = 0;
        for (int i=0; i < clauses; i++) {
            good += m_clauses[i].evaluate(chromosome);
        }
        return good;
    }

    t_individual rouletteSelection(std::vector<t_individual>& pop, int maxFit) {
        int i = 3;
        while(i--){
            for(auto& individual : pop) {
                float r = (float)rand() / RAND_MAX;
                if (((float)individual.first / maxFit) > r)
                    return individual;
            }
        #ifdef DEBUG
            std::cout << "DEBUG | " << "rouletteSelection (" << i << ") Failed Retrying" << std::endl;
        #endif
        }
        return pop[0];
    }

    void crossover(t_individual& p, t_individual& pp) {
        // std::uniform_int_distribution<int> random(0,p.second.size()-1);
        // int point = random(m_generator);
        int point = rand() % p.second.size();
        std::swap_ranges(p.second.begin(), p.second.begin()+point, pp.second.begin());
    }

    void mutation(t_individual& p) {
        // std::uniform_real_distribution<double> random(0,1);
        for (auto& gene : p.second) {
            float r = (float)rand() / RAND_MAX;
            if ((float)m_mutation > r) {
                gene = !gene;
            }
        }
    }

    t_individual evolve() {

        int literals = m_config->getLiterals();
        t_individual best = std::make_pair(0, std::vector<byte>(0));
        std::vector<t_individual> now(m_populationSize);
        
        for (int generation=0; generation < m_generations; generation++) {
            // double generationTime = MPI_Wtime();
            #ifdef DEBUG
            std::cout << "DEBUG | Generation " << generation+1 << std::endl;
            #endif

            

            long sumFitness = 0;

            // #pragma omp parallel for schedule(guided) reduction(+:sumFitness)
            for (auto& individual : now) {
                auto& indFitness = std::get<int>(individual);
                auto& chromosome = std::get<std::vector<byte>>(individual);
                chromosome.resize(literals);
                for(byte& gene : chromosome) {
                    gene = (byte)rand() % 2;
                }
                indFitness = fitness(chromosome);
                sumFitness += indFitness;
            }

            // Momento sequencial obrigatorio ZZZzzz
            std::sort(now.begin(), now.end(), [](t_individual& a, t_individual& b) {
                return std::get<int>(a) > std::get<int>(b);
            });

            best = std::max(best, now.front(), 
            [](const t_individual& best, const t_individual& front) {
                return best.first <= front.first;
            });

            // std::uniform_real_distribution<double> rrandom(0,1);
            std::vector<t_individual> offspring(m_populationSize-5);

            // #pragma omp parallel for schedule(guided)
            for (auto& individual : offspring) {
                t_individual p, pp;
                p = rouletteSelection(now, sumFitness);
                pp = rouletteSelection(now, sumFitness);
                #ifdef DEBUG
                t_individual d = p;
                #endif
                float r = (float)rand() / RAND_MAX;
                if (m_crossover > r)
                    crossover(p, pp);
                mutation(p);
                #ifdef DEBUG
                    std::cout << "DEBUG | ";
                    std::cout << p.second << std::endl;
                    std::cout << "DEBUG | ";
                    std::cout << d.second << std::endl;
                #endif
                individual = p;
            }
            // Elitismo
            #ifdef DEBUG
            std::cout << "now" << now << std::endl;
            std::cout << "off" << offspring << std::endl;
            #endif
            std::swap_ranges(offspring.begin(), offspring.end(), now.begin()+5);
            #ifdef DEBUG
            std::cout << "swappedNow" << now << std::endl;
            std::cout << "swappedOff" << offspring << std::endl;
            #endif
            if (generation % 10 == 0 && m_config->getMpiSize() > 1) {
                int fits[GAMT];
                byte chromo[GAMT*literals];
                int c = 0;
                for (auto& i : now) {
                    fits[c] = i.first;
                    memcpy(&chromo[c*literals], &((i.second)[0]), literals * sizeof(byte));
                    c++;
                    if (c == GAMT) break;
                }
                #ifdef DEBUG
                std::cout << "memcpy chromo: ";
                for (int i=0; i<GAMT*literals; i++) {
                    std::cout << (int)chromo[i] << " ";
                } std::cout << std::endl;
                #endif
                int * outfits = (int*) malloc(GAMT * m_config->getMpiSize() * sizeof(int));
                byte * chromouts = (byte*) malloc(GAMT * literals * m_config->getMpiSize() * sizeof(byte));
                MPI_Allgather(fits, GAMT, MPI_INT, outfits, GAMT, MPI_INT, MPI_COMM_WORLD);
                MPI_Allgather(chromo, GAMT*literals, MPI_CHAR, chromouts, GAMT*literals, MPI_CHAR, MPI_COMM_WORLD);
                #ifdef DEBUG
                std::cout << "Allgathered" << std::endl;
                #endif

                auto it = now.rbegin();
                for (int i=0; i<GAMT; i++) {
                    t_individual temp;
                    temp.first = outfits[i];
                    temp.second.resize(literals);
                    memcpy(&temp.second[0], &chromouts[i*literals], literals * sizeof(byte));
                    *it = temp;
                    it++;
                }
            }

            // std::cout << "Generation time: " << MPI_Wtime() - generationTime << std::endl;
        }
        return best;
    }


};