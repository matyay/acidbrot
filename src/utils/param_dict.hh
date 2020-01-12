#ifndef PARAM_DICT_HH
#define PARAM_DICT_HH

#include <map>
#include <string>

// ============================================================================

/// A parameter dictionary
class ParamDict
{
public:

    /// Default constructor
    ParamDict () = default;
    /// Initializing constructor
    ParamDict (const std::map<std::string, std::string>& a_Params);

    /// Returns true if the parameter is defined.
    bool has (const std::string& a_Name) const;
    /// Deletes a parameter if present
    void del (const std::string& a_Name); 

    /// Returns the parameter if present or the given default value if not.
    std::string get (const std::string& a_Name, const std::string& a_Default = "") const;
    /// Adds / sets the parameter to the given value
    std::string set (const std::string& a_Name, const std::string& a_Value);

protected:

    /// The parameter map
    std::map<std::string, std::string> m_Params;
};

#endif
