#pragma once

#include <bits/stdc++.h>

typedef unsigned char byte;
typedef struct clause {
private:
    byte m_size;
    std::shared_ptr<byte[]> m_literals;
    std::shared_ptr<byte[]> m_operator;
public:
    clause() {

    }
    clause(std::shared_ptr<byte[]>& literals, std::shared_ptr<byte[]>& operators, byte size) {
        #ifdef DEBUG
        std::cout << "clause | " << sizeof(m_literals) << " " << sizeof(m_operator) << " " << sizeof(m_size) << std::endl;
        #endif

        m_literals = literals;
        m_operator = operators;
        m_size = size;
        #ifdef DEBUG
        std::cout << "clause | " << sizeof(m_literals) << " " << sizeof(m_operator) << std::endl;
        #endif
    }
    byte evaluate(std::vector<byte>& instance) {
        for(int i=0; i < m_size; i++) {
            if (instance[m_literals[i]] ^ m_operator[i]) return 1;
        }
        return 0;
    }

} Clause;