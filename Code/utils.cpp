/**
 * @author Bruno Pena Baêta (696997)
 * @author Felipe Nepomuceno Coelho (689661)
 */

#include <sys/stat.h>
#include <string>
#include <algorithm>
#include <set>

/**
 * @brief Checks if a file exists
 * 
 * @param name A string containing the file path
 * @return true if the file exists. false otherwise
 */
inline bool existsFile(const std::string& name) {
  struct stat buffer;
  return (stat (name.c_str(), &buffer) == 0); 
}

template <typename T>
/**
 * @brief Checks if a set contains an element
 * 
 * @param set The set to be checked
 * @param element The element to be checked
 * @return true if the set contains the element. false otherwise
 */
inline bool has(std::set<T> set, T element) {
    return set.find(element) != set.end();
}

template <typename T>
/**
 * @brief Get the difference between two sets. Set1 - Set2
 * 
 * @param set1 The first set
 * @param set2 The second set
 * @return std::set<T> The difference between the two sets
 */
inline std::set<T> getSetDifference(std::set<T> set1, std::set<T> set2) {
    std::set<T> difference;
    std::set_difference(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(difference, difference.begin()));
    return difference;
}

template <typename T>
/**
 * @brief Get the union of two sets
 * 
 * @param set1 The first set
 * @param set2 The second set
 * @return std::set<T> The union of the two sets
 */
inline std::set<T> getSetUnion(std::set<T> set1, std::set<T> set2) {
    std::set<T> unionSet;
    std::set_union(set1.begin(), set1.end(), set2.begin(), set2.end(), std::inserter(unionSet, unionSet.begin()));
    return unionSet;
}

/**
 * @brief Prints a superState
 * 
 * @param ss The superState to be printed
 */
inline void printSuperState(std::set<std::string> ss) {
    std::string text = "{";
    for (auto s : ss) {
        text += s + ", ";
    }
    text = text.substr(0, text.size() - 2);
    text += "}";
    std::cout << text << std::endl;
}