/**
 * @author Bruno Pena Baêta (696997)
 * @author Felipe Nepomuceno Coelho (689661)
 */

#include <iostream>
#include "pugixml/pugixml.hpp"
#include <chrono>

// #define BASE_PATH "./../" // Debug path
#define BASE_PATH "./../../" // Execution path

DFA loadDfaFromFile(bool* dfaNullFlag);
void exportDfaToFile(DFA dfa);
DFA minimizeWithON2Algorithm(DFA dfa);
DFA minimizeWithONLogNAlgorithm(DFA dfa);
DFA generateDfa(int n);

int main()
{
    DFA dfa = DFA();
    bool dfaNullFlag = true;
    bool quit = false;

    while (!quit) {
        std::cout << "MENU:\n1. Load DFA file\n2. Export DFA\n3. Run O(n^2) Algorithm\n4. Run O(n log n) Algorithm\n5. Generate n states DFA\n0. Quit\nChoose option: ";
        int option;
        std::cin >> option;
        switch (option) {
        case 1:
            dfa = loadDfaFromFile(&dfaNullFlag);
            break;
        case 2:
            if (dfaNullFlag) {
                std::cout << "\nNo DFA loaded yet.\n\n";
                break;
            }
            exportDfaToFile(dfa);
            break;
        case 3:
            if (dfaNullFlag) {
                std::cout << "\nNo DFA loaded yet.\n\n";
                break;
            }
            try {
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                dfa = minimizeWithON2Algorithm(dfa);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                std::cout << "Total time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms\n\n";
            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
            break;
        case 4: 
            if (dfaNullFlag) {
                std::cout << "\nNo DFA loaded yet.\n\n";
                break;
            }
            try {
                std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
                dfa = minimizeWithONLogNAlgorithm(dfa);
                std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
                std::cout << "Total time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms\n\n";
            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
            break;
        case 5:
            std::cout << "Number of states: ";
            int n;
            std::cin >> n;
            if (n > 0) {
                dfa = generateDfa(n);
                dfaNullFlag = false;
            } else {
                std::cout << "\nInvalid number of states.\n\n";
            }
            break;
        default:
            quit = true;
            break;
        }
    }

    return 0;
}

/**
 * @brief Loads a DFA from a file
 * 
 * @param dfaNullFlag Pointer to a flag that indicates if the DFA is null
 * @return The DFA loaded from the file
 */
DFA loadDfaFromFile(bool* dfaNullFlag) {
    std::cout << "File name to load: ";
    std::string file_name;
    std::cin >> file_name;

    std::cout << "Loading file...\n";

    std::string s_base_path = BASE_PATH;
    std::string file_path = s_base_path + "Data/" + file_name;

    if (!existsFile(file_path)) {
        std::cout << "\nFile not found.\n\n";
        *dfaNullFlag = true;
        return DFA();
    }

    pugi::xml_document file;
    pugi::xml_parse_result result = file.load_file(file_path.c_str());

    if (!result) {
        std::cout << "\nError loading file.\n\n";
        *dfaNullFlag = true;
        return DFA();
    }
    
    std::cout << "File loaded successfully.\n";

    std::cout << "Setting up DFA...\n";

    DFA dfa = DFA();
    pugi::xml_node automaton = file.child("structure").child("automaton");

    // Setting up states
    for (pugi::xml_node node = automaton.child("state"); node; node = node.next_sibling("state")) {
        std::string id = node.attribute("id").value();
        dfa.addState(id);
        if (node.child("initial")) {
            dfa.setInitialState(id);
        }
        if (node.child("final")) {
            dfa.addFinalState(id);
        }
    }

    // Setting up transitions
    for (pugi::xml_node node = automaton.child("transition"); node; node = node.next_sibling("transition")) {
        std::string symbol = node.child_value("read");
        dfa.addSymbol(symbol);
        dfa.addTransition(node.child_value("from"), symbol, node.child_value("to"));
    }

    std::cout << "DFA successfully setted up.\n\n";

    *dfaNullFlag = false;
    return dfa;
}

/**
 * @brief Exports a DFA to a file
 * 
 * @param dfa The DFA to be exported
 */
void exportDfaToFile(DFA dfa) {
    std::cout << "File name to export: ";
    std::string file_name;
    std::cin >> file_name;

    std::string s_base_path = BASE_PATH;
    std::string file_path = s_base_path + "Output/" + file_name;

    if (existsFile(file_path)) {
        std::cout << "\nFile already exists.\n\n";
        return;
    }

    std::cout << "Loading skeleton file...\n";

    std::string skeleton_path = s_base_path + "Data/skeleton.jff";
    if (!existsFile(skeleton_path)) {
        std::cout << "\nSkeleton file not found. Please recreate it.\n\n";
        return;
    }
    pugi::xml_document skeleton;
    pugi::xml_parse_result result = skeleton.load_file(skeleton_path.c_str());
    if (!result) {
        std::cout << "\nError loading skeleton file. Please recreate it.\n\n";
        return;
    }

    std::cout << "Exporting DFA...\n";
    
    pugi::xml_node automaton = skeleton.child("structure").child("automaton");

    // Setting up states
    for (state s : dfa.getStates()) {
        pugi::xml_node state = automaton.append_child("state");
        state.append_attribute("id") = s.c_str();
        state.append_attribute("name") = ("q" + s).c_str();
        state.append_child("x").append_child(pugi::node_pcdata).set_value("0");
        state.append_child("y").append_child(pugi::node_pcdata).set_value("0");
        if (s == dfa.getInitialState()) {
            state.append_child("initial");
        }
        if (dfa.isFinalState(s)) {
            state.append_child("final");
        }
    }

    // Setting up transitions
    for (std::pair<const transition, state> const& it : dfa.getTransitions()) {
        pugi::xml_node transition = automaton.append_child("transition");
        const pugi::char_t* state1 = it.first.first.c_str();
        const pugi::char_t* state2 = it.second.c_str();
        const pugi::char_t* symbol = it.first.second.c_str();
        transition.append_child("from").append_child(pugi::node_pcdata).set_value(state1);
        transition.append_child("to").append_child(pugi::node_pcdata).set_value(state2);
        transition.append_child("read").append_child(pugi::node_pcdata).set_value(symbol);
    }

    skeleton.save_file(file_path.c_str());

    std::cout << "\nDFA successfully exported to " + file_path + ".\n\n";
}

/**
 * @brief Runs an O(n^2) algorithm that minimizes a DFA. This algorithm was created by Blum (1996).
 * 
 * @param dfa The DFA to be minimized
 * 
 * @return The minimized DFA
 */
DFA minimizeWithON2Algorithm(DFA dfa) {
    return myOn2Algorithm(dfa);
}

/**
 * @brief Runs an O(n log n) algorithm that minimizes a DFA. This algorithm was created by Blum (1996), it is a modification of the O(n^2) algorithm.
 * 
 * @param dfa The DFA to be minimized
 * 
 * @return The minimized DFA
 */
DFA minimizeWithONLogNAlgorithm(DFA dfa) {
    return blumOnLognAlgorithm(dfa);
}

/**
 * @brief Generates a DFA with n states. The DFA forces the worst case to the O(n^2) algorithm.
 * 
 * @param n The number of states
 * @return The generated DFA
 */
DFA generateDfa(int n) {
    DFA dfa = DFA();
    dfa.addSymbol("a");
    for (int i = 0; i < n; i++) {
        dfa.addState(std::to_string(i));
    }
    dfa.setInitialState("0");
    dfa.addFinalState(std::to_string(n-1));
    for (int i = 0; i < n; i++) {
        dfa.addTransition(std::to_string(i), "a", std::to_string((i + 1) % n));
    }
    return dfa;
}