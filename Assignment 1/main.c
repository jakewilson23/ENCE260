#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "float24.h"

int main(void)
{
    //make a array of Float24_t of size 10 initially using calloc
    int8_t inputArraySize = 10;
    Float24_t* inputArray = calloc(inputArraySize, sizeof(Float24_t));

    int8_t loopCounter = 0;
    int8_t validInput = 1; //1 = valid input, 0 = not valid input
    //stop loop if there is invalid input or 100 items in the inputArray
    while (validInput && loopCounter < 100) {
        Float24_t tempInput = float24_read();
        //If a malformed Float24_t was returned stop the loop and dont add it to the array
        if ((tempInput.mantissa == 0) && (tempInput.exponent == -128)) {
            validInput = 0;
        } else {
            if (loopCounter % 10 == 9) {
                //add 10 more memory to array if the amount of elements ends with a 9, 9,19,29,39 etc
                inputArraySize += 10;
                inputArray = realloc(inputArray, inputArraySize*sizeof(Float24_t));
            }
            //put the returned Float24_t in the array
            inputArray[loopCounter] = tempInput;
            loopCounter ++;
        }
    }
    //initialise a Float to hold the accumulated sum of all floats in the array
    Float24_t accumulatedSum = float24_init(0,0);
    //iterate through the array to print each float and add it to the accumulated sum
    for (int i = 0; i < loopCounter; i++) {
        printf("Array[%d]: %f\n", i, float24_asIEEE(inputArray[i]));
        accumulatedSum = float24_add(accumulatedSum, inputArray[i]);
    }
    //print the accumulated sum
    printf("Accumulated Sum: %f\n", float24_asIEEE(accumulatedSum));

    //square the accumulated sum and print it
    accumulatedSum = float24_multiply(accumulatedSum, accumulatedSum);
    printf("Square Accumulated Sum: %f\n", float24_asIEEE(accumulatedSum));

    //run the array max function to find the max value in the array and print it
    Float24_t* max = float24_arrayMax(inputArray, loopCounter, &float24_max);
    printf("Max of Numbers: %f\n", float24_asIEEE(*max));

    //free the memory for input array
    free(inputArray);

}