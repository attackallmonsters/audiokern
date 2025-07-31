#pragma once

#include "DSPObject.h"
#include "dsp_types.h"
#include <cstdlib>
// other includes

/**
 * @brief A short and concise description of the class
 *
 * A detailed description of the purpose of the class
 *
 * Usage: (Example use of the methods)
 * 
 * - Call this method for ...
 * - Call other method for ...
 * 
 * Put useful Backgound information here
 * 
 * Example:
 * Code excamples are placed here
 *   ...
 */
class ExcampleClass : public DSPObject
{
public:
    ExcampleClass(); // Each class has a constructor for initialization of basic data
    ~ExcampleClass();   /// Each class has a destructor for releasing objects

    /**
     * @brief A short and concise description of the public member
     * 
     * A detailed description of the purpose of the method
     *
     * @param a Parameter description
     */
    void setValue(host_float v);

protected:
    /**
     * @brief A short and concise description of the protected member
     */
    void initializeObject() override; //Initializes a DSP object and sets meaningful default values 

private:
    /// @brief Static dispatcher for DSP block processing
    static void processBlock(DSPObject *dsp);

    /// @brief Internal block processing
    void processBlock();

    /// @brief A short and concise description of the private member
    host_float var = 0.0f;
};
