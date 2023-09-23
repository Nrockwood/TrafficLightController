////////////////////////////////////////////////////////////
///  @file ExampleApp.h
///  @author rockwood
///  @brief Example App using an IExampleInterface
///  @version 0.1
///  @date 2022-11-08
///  
///  @copyright Copyright (c) 2022
///  
////////////////////////////////////////////////////////////

#ifndef INCLUDE_EXAMPLEAPP_H_
#define INCLUDE_EXAMPLEAPP_H_

#include "interfaces/example/IExampleInterface.h"

#include <cstdint>

using namespace foo;

////////////////////////////////////////////////////////////
///  @brief Example App implementation 
///  
////////////////////////////////////////////////////////////
class ExampleApp : public IExampleInterface<uint64_t>
{
    public:
        ExampleApp();

        void initApp() override;

        void run() override;

        uint64_t currentTime() override;

    private:
        ////////////////////////////////////////////////////////////
        ///  @brief Prints Hello World from this app
        ///  
        ////////////////////////////////////////////////////////////
        void printHelloWorld();

        ////////////////////////////////////////////////////////////
        ///  @brief Performs addition of leftValue & rightValue
        ///  
        ///  @param leftValue Left value in addition
        ///  @param rightValue Right value in addition
        ///  @param sum Result of sum
        ////////////////////////////////////////////////////////////
        const void calculateSum(int32_t leftValue, int32_t rightValue, int64_t& sum);

        /// Members
        bool appState_;
};

#endif // INCLUDE_EXAMPLEAPP_H_
