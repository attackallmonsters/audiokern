#include "m_pd.h"
#include "pdbase.h"
#include <iostream>

void log(const std::string &entry)
{
    post("%s", entry.c_str());
}