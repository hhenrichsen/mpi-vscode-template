#ifndef MPI_U_HPP
#define MPI_U_HPP

#include <iostream>
#include <ostream>
#include <mpi.h>
#include <stdlib.h>
#include <math.h>
#include <functional>

//
// MPI Debug Macros
//

#define debug(a) debug_print(rank, size, #a, a, "")

#define debugf(a, m) debug_print(rank, size, #a, a, m)

#define debugh(m) debug_header(rank, m)

#define debugt(a) debug_table(rank, size, #a, a)

//
// Constants
//

#define MCW MPI_COMM_WORLD

#define FILTER -1

//
// Useful Functions
//

/**
 * Returns std::cout if the filter values are equal. Otherwise, returns an
 * empty ostream.
 * 
 * @param a One of the values to be compared.
 * @param b One of the values to be compared.
 * 
 * @return std::cout if a == b, otherwise an empty ostream.
 */
std::ostream& filter_ios(int a, int b);

/**
 * Returns std::cout if the filter matches the value above. Otherwise, returns
 * an empty ostream.
 * 
 * @param filter One of the values to be compared.
 * 
 * @return std::cout if the filters match, otherwise an empty ostream.
 */
std::ostream& filter_ios(int filter);

/**
 * Prints a debug header on ONLY the process when rank == 1;
 * 
 * An alias debugh exists that automatically fills rank.
 * 
 * @param rank The rank of the process.
 * @param header The header to print.
 */
void debug_header(int rank, std::string header);

/**
 * Prints a list of values from each process, collected into process 0 and then
 * printed in order.
 * 
 * An alias debugp exists that automatically fills everything except the
 * variable name.
 * 
 * @param rank The rank of this process.
 * @param size The number of processes.
 * @param name The name of the data being displayed.
 * @param data The data being displayed.
 * @param marker A message to be printed before the data.
 */
void debug_print(int rank, int size, std::string name, const int data, std::string marker);

/**
 * Similar to debug_print, except that it prints the data in a nice tabular 
 * format.
 * 
 * An alias debugt exists that automatically fills everything except the
 * variable name.
 * 
 * @param rank The rank of this process.
 * @param size The number of processes.
 * @param name The name of the data being displayed.
 * @param data The data being displayed.
 */
void debug_table(int rank, int size, std::string name, const int data);

/**
 * Finds the max value in an array of arbitrary size.
 * 
 * @param arr The array to look through.
 * @param size The array size.
 * 
 * @return The max value in the array.
 */
template <typename T>
T max_val_in(T* arr, int size) {
    T maxVal;
    for (int i = 0; i < size; i++) {
        maxVal = std::max(maxVal, arr[i]);
    }
    return maxVal;
}

/**
 * Finds the max value in an array of arbitrary size.
 * 
 * @param arr The array to look through.
 * @param size The array size.
 * @param init The initial value.
 * 
 * @return The max value in the array.
 */
template <typename T>
T max_val_in(T* arr, int size, T init) {
    T maxVal = init;
    for (int i = 0; i < size; i++) {
        maxVal = std::max(maxVal, arr[i]);
    }
    return maxVal;
}

/**
 * Centers a string, filling both ends with fill_char.
 * 
 * @param msg The original string.
 * @param total The desired length.
 * @param fill_char The character to fill either side with.
 * 
 * @return The centered string.
 */
std::string center_string(std::string msg, int total, std::string fill_char);

/**
 * Centers a string, filling both ends with spaces.
 * 
 * @param msg The original string.
 * @param total The desired length.
 * 
 * @return The centered string.
 */
std::string center_string(std::string msg, int total);

/**
 * Duplicates a string a given number of times.
 * 
 * @param msg The original string.
 * @param total The number of times to duplicate.
 * 
 * @return The duplicated string.
 */
std::string string_times(std::string msg, int total);

/**
 * Helper function mostly, but included because it might be useful. Prints a 
 * line in a tabular layout, with specified characters at certain points. Uses
 * a provider in order to allow for flexible use cases such as pulling from an
 * array or generating numbers on the fly.
 * 
 * @param leftCap The character that begins the string.
 * @param mid The character that splits up items in the table.
 * @param rightCap The character that terminates the string.
 * @param fill_char The character that fills spaces in cells.
 * @param provider Provides the values for the table.
 * @param size The number of columns.
 * @param col_size The width of each columns.
 */
void print_table_row(std::string leftCap, std::string mid, std::string rightCap, 
std::string fill_char, std::function<std::string (int)> provider, int size, int col_size);

#endif