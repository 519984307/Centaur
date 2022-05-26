/////////////////////////////////////////////////////////////////////////////////////
//
// Created by Ricardo Romero on 25/05/22.
// Copyright (c) 2022 Ricardo Romero.  All rights reserved.
//

#pragma once

#ifndef __cplusplus
#error "C++ compiler needed"
#endif /*__cplusplus*/

#ifndef CENTAUR_XMLHELPER_HPP
#define CENTAUR_XMLHELPER_HPP

#include "../../Centaur.hpp"
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/XMLString.hpp>

namespace CENTAUR_NAMESPACE
{
    struct XMLStr
    {
        explicit XMLStr(const char *ptr) :
            m_ptr { xercesc::XMLString::transcode(ptr) } { }
        ~XMLStr()
        {
            xercesc::XMLString::release(&m_ptr);
        }
        operator XMLCh *() const
        {
            return m_ptr;
        }

    private:
        XMLCh *m_ptr;
    };
    struct StrXML
    {
        explicit StrXML(const XMLCh *ptr) :
            m_ptr { xercesc::XMLString::transcode(ptr) } { }
        ~StrXML()
        {
            xercesc::XMLString::release(&m_ptr);
        }
        operator char *() const
        {
            return m_ptr;
        }

    private:
        char *m_ptr;
    };
} // namespace CENTAUR_NAMESPACE

#endif // CENTAUR_XMLHELPER_HPP
