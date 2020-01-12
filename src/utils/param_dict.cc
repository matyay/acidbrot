#include "param_dict.hh"

// ============================================================================

ParamDict::ParamDict (const std::map<std::string, std::string>& a_Params) :
    m_Params(a_Params)
{
    // Empty
}

// ============================================================================

bool ParamDict::has (const std::string& a_Name) const {
    return m_Params.count(a_Name) != 0;
}

void ParamDict::del (const std::string& a_Name) {
    auto itr = m_Params.find(a_Name);

    if (itr != m_Params.end()) {
        m_Params.erase(itr);
    }
}

std::string ParamDict::get (const std::string& a_Name, const std::string& a_Default) const {
    auto itr = m_Params.find(a_Name);

    if (itr != m_Params.end()) {
        return itr->second;
    }
    else {
        return a_Default;
    }
}

std::string ParamDict::set (const std::string& a_Name, const std::string& a_Value) {
    m_Params[a_Name] = a_Value;
}
