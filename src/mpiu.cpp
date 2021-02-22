#include "mpiu.hpp"
#include <sstream>
//
// Various Debug-Printing options. 
//
std::ostream hidden_stream(0);

/**
 * Returns std::cout if the filter values are equal. Otherwise, returns an
 * empty ostream. Also returns std::cout if b is negative.
 * 
 * @param a One of the values to be compared.
 * @param b One of the values to be compared.
 * 
 * @return std::cout if a == b, otherwise an empty ostream.
 */
std::ostream& filter_ios(int a, int b) {
    if(a == b || b < 0) {
        return std::cout;
    }
    return hidden_stream;
}

/**
 * Returns std::cout if the filter matches the value above. Otherwise, returns
 * an empty ostream.
 * 
 * @param filter One of the values to be compared.
 * 
 * @return std::cout if the filters match, otherwise an empty ostream.
 */
std::ostream& filter_ios(int filter_val) {
    return filter_ios(filter_val, FILTER);
}

/**
 * Prints a debug header on ONLY the process when rank == 1;
 * 
 * An alias debugh exists that automatically fills rank.
 * 
 * @param rank The rank of the process.
 * @param header The header to print.
 */
void debug_header(int rank, std::string header) {
    if (rank == 0) {
        std::cout << header << std::endl;
    }
}

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
void debug_print(int rank, int size, std::string name, const int data, std::string marker) {
    MPI_Barrier(MCW);
    int* recv = new int[size];
    MPI_Gather(&data, 1, MPI_INT, recv, 1, MPI_INT, 0, MCW);
    if (rank == 0) {
        for (int i = 0; i < size; i++) {
            filter_ios(i) << marker << i << " " << name << ": " << recv[i] << std::endl;
        }
    }
    delete[] recv;
    MPI_Barrier(MCW);
}

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
void debug_table(int rank, int size, std::string name, const int data) {
    MPI_Barrier(MCW);
    int* recv = new int[size];
    MPI_Gather(&data, 1, MPI_INT, recv, 1, MPI_INT, 0, MCW);

    int maxIdLen = std::log10(size-1);
    int maxValLen = std::log10(max_val_in(recv, size, 0));
    int col_size = std::max(maxIdLen, maxValLen) + 3;
    int totalLen = (1 + col_size) * size - 1;
    if (rank == 0) {
        print_table_row("┌", "─", "┐", "─", [](int i) -> std::string { return ""; }, size, col_size);
        std::cout << "│" << center_string(name, totalLen) << "│" << std::endl;
        print_table_row("├", "┬", "┤", "─", [](int i) -> std::string { return ""; }, size, col_size);
        print_table_row("│", "│", "│", " ", [](int i) -> std::string { return std::to_string(i); }, size, col_size);
        print_table_row("├", "┼", "┤", "─", [](int i) -> std::string { return ""; }, size, col_size);
        print_table_row("│", "│", "│", " ", [recv](int i) -> std::string { return std::to_string(recv[i]); }, size, col_size);
        print_table_row("└", "┴", "┘", "─", [](int i) -> std::string { return ""; }, size, col_size);
    }

    delete[] recv;
    MPI_Barrier(MCW);
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
std::string center_string(std::string msg, int total, std::string fill_char) {
    std::ostringstream ss;
    int o_len = msg.length();
    int diff = total - o_len;
    int front = (diff / 2 + (diff % 2));
    int back = (diff / 2);
    for (int i = 0; i < front; i++) {
        ss << fill_char;
    }
    ss << msg;
    for (int i = 0; i < back; i++) {
        ss << fill_char;
    }
    return ss.str();
}

/**
 * Centers a string, filling both ends with spaces.
 * 
 * @param msg The original string.
 * @param total The desired length.
 * 
 * @return The centered string.
 */
std::string center_string(std::string msg, int total) {
    return center_string(msg, total, " ");
}

/**
 * Duplicates a string a given number of times.
 * 
 * @param msg The original string.
 * @param total The number of times to duplicate.
 * 
 * @return The duplicated string.
 */
std::string string_times(std::string msg, int total) {
    std::stringstream ss;
    for (int i = 0; i < total; i++) {
        ss << msg;
    }
    return ss.str();
}

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
void print_table_row(std::string leftCap, std::string mid, std::string rightCap, std::string fill_char, std::function<std::string (int)> provider, int size, int col_size) {
    std::cout << leftCap;
    for (int i = 0; i < size; i++) {
        std::string data = provider(i);
        std::cout << (i != 0 ? mid : "") << center_string(data, col_size, fill_char);
    }
    std::cout << rightCap << std::endl;
}