/*******************************************************
 * Copyright (c) 2014, ArrayFire
 * All rights reserved.
 *
 * This file is distributed under 3-clause BSD license.
 * The complete license agreement can be obtained at:
 * http://arrayfire.com/licenses/BSD-3-Clause
 ********************************************************/

#pragma once
#include "Node.hpp"
#include <iomanip>

namespace opencl
{

namespace JIT
{

    class BinaryNode : public Node
    {
    private:
        std::string m_op_str;
        Node *m_lhs, *m_rhs;
        int m_op;

    public:
        BinaryNode(const char *out_type_str,
                   const char *op_str,
                   Node *lhs, Node *rhs, int op)
            : Node(out_type_str),
              m_op_str(op_str),
              m_lhs(lhs),
              m_rhs(rhs),
              m_op(op)
        {
            lhs->addParent(this);
            rhs->addParent(this);
        }

        void replaceChild(Node *prev, Node *curr)
        {
            if (m_lhs == prev) m_lhs = curr;
            else m_lhs->replaceChild(prev, curr);

            if (m_rhs == prev) m_rhs = curr;
            else m_rhs->replaceChild(prev, curr);
        }

        void genParams(std::stringstream &kerStream)
        {
            if (m_gen_param) return;
            if (!(m_lhs->isGenParam())) m_lhs->genParams(kerStream);
            if (!(m_rhs->isGenParam())) m_rhs->genParams(kerStream);
            m_gen_param = true;
        }

        int setArgs(cl::Kernel &ker, int id)
        {
            id = m_lhs->setArgs(ker, id);
            id = m_rhs->setArgs(ker, id);
            return id;
        }

        void genOffsets(std::stringstream &kerStream)
        {
            if (m_gen_offset) return;
            if (!(m_lhs->isGenOffset())) m_lhs->genOffsets(kerStream);
            if (!(m_rhs->isGenOffset())) m_rhs->genOffsets(kerStream);
            m_gen_offset = true;
        }

        void genKerName(std::stringstream &kerStream, bool genInputs)
        {
            if (!genInputs) {
                // Make the hex representation of enum part of the Kernel name
                kerStream << std::setw(2) << std::setfill('0') << std::hex << m_op << std::dec;
            }
            m_lhs->genKerName(kerStream, genInputs);
            m_rhs->genKerName(kerStream, genInputs);
        }

        void genFuncs(std::stringstream &kerStream)
        {
            if (m_gen_func) return;

            if (!(m_lhs->isGenFunc())) m_lhs->genFuncs(kerStream);
            if (!(m_rhs->isGenFunc())) m_rhs->genFuncs(kerStream);

            kerStream << m_type_str << " val" << m_id << " = "
                      << m_op_str << "(val" << m_lhs->getId()
                      << ", val" << m_rhs->getId() << ");"
                      << std::endl;

            m_gen_func = true;
        }

        int setId(int id)
        {
            if (m_set_id) return id;

            id = m_lhs->setId(id);
            id = m_rhs->setId(id);

            m_id = id;
            m_set_id = true;

            return m_id + 1;
        }

        void resetFlags()
        {
            m_set_id = false;
            m_gen_func = false;
            m_gen_param = false;
            m_gen_offset = false;
            m_lhs->resetFlags();
            m_rhs->resetFlags();
        }
    };

}

}
