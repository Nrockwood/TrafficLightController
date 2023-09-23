////////////////////////////////////////////////////////////
///  @file ExampleApp.cpp
///  @author rockwood
///  @brief Example App Implementation
///  @version 0.1
///  @date 2022-11-08
///  
///  @copyright Copyright (c) 2022
///  
////////////////////////////////////////////////////////////

#include "impl/example/ExampleApp.h"

#include <iostream>

ExampleApp::ExampleApp()
    : appState_(false)
{
    std::cout << "Constructed ExampleApp." << std::endl;
}

void ExampleApp::initApp()
{
    appState_ = true;

    std::cout << "Initialized ExampleApp." << std::endl;
}

void ExampleApp::run()
{
    printHelloWorld();

    std::cout << "It is currently " << currentTime() << std::endl;

    int64_t whatIsTheMeaningOfLife = 0;
    calculateSum(2, 40, whatIsTheMeaningOfLife);

    std::cout << "Meaning of life is..." << whatIsTheMeaningOfLife << std::endl;
}

uint64_t ExampleApp::currentTime()
{
    time_t currentTime;
    currentTime = time(&currentTime);

    return static_cast<uint64_t>(currentTime);
}

void ExampleApp::printHelloWorld()
{
    std::cout << "HelloWorld From ExampleApp!" << std::endl;
}

const void ExampleApp::calculateSum(int32_t leftValue, int32_t rightValue, int64_t& sum)
{
    sum = leftValue + rightValue;
}
