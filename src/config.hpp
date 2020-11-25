#pragma once
#include <bits/stdc++.h>

typedef struct config {
    std::string m_filename;
    int m_literals;
    int m_clauses;
    int m_sat;
    int m_mpi_size;
    int m_mpi_rank;
    public:
    config(std::string filename, int literals, int clauses, int sat, int mpi_size, int mpi_rank) : 
        m_filename(filename), m_literals(literals), m_clauses(clauses), m_sat(sat), m_mpi_size(mpi_size), m_mpi_rank(mpi_rank)
    {}
    std::string getFileName() {
        return m_filename;
    }
    int getLiterals() {
        return m_literals;
    }
    int getClauses() {
        return m_clauses;
    }
    int getSat() {
        return m_sat;
    }
    int getMpiSize() {
        return m_mpi_size;
    }
    int getMpiRank() {
        return m_mpi_rank;
    }
} Config;