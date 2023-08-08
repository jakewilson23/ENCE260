#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h> //INT16_MAX, -INT16_MAX
#include <math.h>
#include <ctype.h>

#include "float24.h"

//typedef struct {
//    int16_t mantissa;
//    int8_t exponent;
//} Float24_t;

Float24_t float24_init(int16_t mantissa, int8_t exponent)
//initialise a Float24 with a given mantissa and exponent and return it
{
    Float24_t newFloat;
    newFloat.mantissa = mantissa;
    newFloat.exponent = exponent;
    return newFloat;
}

void float24_print(Float24_t value)
//Print a Float24 in the form (mantissa * 2 ^ exponent)
{
    printf("%d * 2 ^ %d\n", value.mantissa, value.exponent);
}

static Float24_t float24_normalise(int32_t oversizeMantissa, int8_t exponent)
{
    //if the mantissa is bigger than 32767 or less than -32767, divide it and check again
    while (oversizeMantissa > INT16_MAX || oversizeMantissa < -INT16_MAX) {
        //everytime we divide the mantissa we add 1 to the exponent
        oversizeMantissa = oversizeMantissa / 2;
        exponent += 1;
    }
    //At this point the int32_t will be a value representable by an int16_t
    int16_t tempMantissa = oversizeMantissa;
    //initialize and return a new Float24_t
    Float24_t newFloat;
    newFloat.mantissa = tempMantissa;
    newFloat.exponent = exponent;
    return newFloat;
}

Float24_t float24_multiply(Float24_t num1, Float24_t num2)
//a * b = (a1 * b1) * 2 ^ (a2+b2)
{
    int32_t tempMantissa = (num1.mantissa * num2.mantissa);
    int8_t tempExponent =  (num1.exponent + num2.exponent);
    Float24_t result = float24_normalise(tempMantissa, tempExponent);
    return result;
}

Float24_t float24_add(Float24_t num1, Float24_t num2)
//a + b = (a'1 + b'1) * 2^c, where c = max(a2,b2) and a = a'1 * 2^c and b = b'1 * 2^c
{
    //printf("num1Mant: %d, num2Mant: %d\n", num1.mantissa, num2.mantissa);

    int8_t exponentDifference;
    int8_t tempExponent = 0;
    int32_t tempMantissa;

    //Find the absolute value of the difference between the two exponents
    exponentDifference = abs(num1.exponent - num2.exponent);

    if (num1.exponent >= num2.exponent) {
        //num1.exponent is the biggest or equal exponent
        tempExponent = num1.exponent;
        //convert the smallest exponent to the biggest by 2 to the power of the absolute difference
        tempMantissa = (num1.mantissa + floor(num2.mantissa/pow(2,exponentDifference)));
    } else {
        //num2.exponent is the biggest or equal exponent
        tempExponent = num2.exponent;
        //convert the smallest exponent to the biggest by 2 to the power of the absolute difference
        tempMantissa = (floor(num1.mantissa/pow(2,exponentDifference)) + num2.mantissa);
    }

    //printf("MANTISSA: %d, Exponent: %d\n", tempMantissa, tempExponent);
    Float24_t result = float24_normalise(tempMantissa, tempExponent);
    return result;
}

int8_t convertInt32toInt8(int32_t input, int8_t isNegative)
//if int32 input is over 127, eg cant fit in int8, set it to 127 and return it
// * the value by isNegative which will either be 1 or -1
{
    int8_t result = 0;
    if (input > 127) {
        result = 127;
    } else {
        result = input;
    }
    return result * isNegative;
}

Float24_t float24_read(void)
//Reads A legitimate float24 value string of the form: "mantissabexponent", b separating the two
{
    int8_t isNegative = 1; //1 = Positive, -1 = Negative
    int8_t mantOrExpo = 0; // 0 = Mantissa, 1 = Exponent
    int32_t tempMantissa = 0;
    int32_t tempExponent = 0;
    int8_t validInput = 1;
    int8_t numberInputReceived = 0;
    int counter = 0;
    char tempChar;
    //Mantissa Loop
    while(validInput) {
        tempChar = getchar();
        //check if the input is valid
        if (isdigit(tempChar)) {
            if (mantOrExpo == 0) {
                //Every digit added pushes the existing mantissa to the left 1, etc 42 becomes 420 and new char is added
                tempMantissa = (tempMantissa * pow(10,1)) + (tempChar - 48);    //-48 because of ascii
            } else {
                tempExponent = (tempExponent * pow(10,1)) + (tempChar - 48);
            }
            numberInputReceived = 1;
        } else if (counter == 0 && (tempChar == '+' || tempChar == '-')) {
            // + or - only accepted as the first input of both mantissa and exponent
            if (tempChar == '-') {
                isNegative = -1;
            }
        } else {
            if ((mantOrExpo == 0) && (tempChar == 'b')) {
                //mantissa loop is over, apply negative is needed and reset loop default values
                tempMantissa *= isNegative;
                isNegative = mantOrExpo = 1;  //reset negative and set loop to expo
                counter = -1; // -1 so counter in loop will +1 to get it to 0 which is reset value
                numberInputReceived = 0;
            } else {
                if ((mantOrExpo == 1) && (numberInputReceived == 1)) {
                    //we are in exponent loop and a number input has been received, so return the input values
                    return float24_normalise(tempMantissa, (convertInt32toInt8(tempExponent, isNegative)));
                }
                validInput = 0;
            }   
        }
        counter += 1;
    }
    return float24_normalise(0, -128);
}

void float24_max(Float24_t* num1, Float24_t* num2, Float24_t** max)
{
    //printf("Hi, we are in the max function\n");
    //printf("Num1: %p, num2: %p max: %p\n", num1, num2, *max);
    //printf("num1 Mantissa: %d, num1 Exponent: %d\n", num1->mantissa, num1->exponent);
    //printf("num2 Mantissa: %d, num2 Exponent: %d\n", num2->mantissa, num2->exponent);
    int32_t tempNum1 = floor(num1->mantissa * pow(2,num1->exponent));
    int32_t tempNum2 = floor(num2->mantissa * pow(2,num2->exponent));
    //printf("num1: %d, num2: %d\n", tempNum1, tempNum2);
    if (tempNum1 > tempNum2) {
        //num1 is the max
        *max = num1;
    } else if (tempNum2 > tempNum1) {
        //num2 is the max
        *max = num2;
    } else {
        //the two values are equal, the max is the one with the largest exponent
        if (num1->exponent >= num2->exponent) {
            //num1 exponent is greater than or equal so num1 is max
            *max = num1;
        } else {
            //num2 exponent is largest so num2 is max
            *max = num2;
        }
    }
}

Float24_t* float24_arrayMax(Float24_t* array, size_t size, void (*func)(Float24_t*, Float24_t*, Float24_t**))
{
    Float24_t* max;
    for (int i = 0; i < size; i++) {
        if (i == 0) {
            max = &array[i];
        } else {
            func(max, &array[i], &max);
        }
    }
    return max;
}