/**
 * @author Bruno Pena Baêta (696997)
 * @author Felipe Nepomuceno Coelho (689661)
 */

#include <iostream>
#include "pugixml/pugixml.hpp"
#include "algorithms.cpp"
#include <chrono>
#include <fstream>

// #define BASE_PATH "./../" // Debug path
#define BASE_PATH "./../../" // Execution path


FA loadDfaFromFile(bool* faNullFlag);
FA loadDfaFromERFile(bool* faNullFlag);
std::string treatExpression(std::string expression);
std::string treatStringChar(std::string stringChar);
void exportDfaToFile(FA fa);
FA minimizeDFA(FA fa);
FA generateDfa(int n);
FA loadFAFromRegularExpression(bool* faNullFlag, std::string regular_expression);
FA transformNfaToDfa(FA fa);
void testMultipleSentences(FA fa);

int main()
{
    FA fa = FA();
    bool faNullFlag = true;
    bool quit = false;

    while (!quit) {
        std::cout << "MENU:\
        \n1. Load FA from Regular Expression\
        \n2. Load FA from ER file\
        \n3. Transform NFA to DFA\
        \n4. Export FA to XML file\
        \n5. Test single sentence\
        \n6. Test multiple sentences\
        \n0. Quit\
        \nChoose option: ";
        int option;
        std::cin >> option;
        switch (option) {
        case 1:
            {
                std::cout << "Regular Expression: ";
                std::string regular_expression;
                std::cin >> regular_expression;
                regular_expression = treatExpression(regular_expression);
                fa = loadFAFromRegularExpression(&faNullFlag, regular_expression);
                break;
            }
        case 2:
            fa = loadDfaFromERFile(&faNullFlag);
            break;
        case 3:
            if (faNullFlag) {
                std::cout << "\nNo FA loaded yet.\n\n";
                break;
            }
            fa = transformNfaToDfa(fa);
            break;
        case 4:
            if (faNullFlag) {
                std::cout << "\nNo FA loaded yet.\n\n";
                break;
            }
            exportDfaToFile(fa);
            break;
        case 5:
            {
                if (faNullFlag) {
                    std::cout << "\nNo FA loaded yet.\n\n";
                    break;
                }
                std::cout << "Sentence to test: ";
                std::string sentence;
                std::cin >> sentence;
                if (fa.testSentence(sentence)) {
                    std::cout << "\nSentence accepted.\n\n";
                } else {
                    std::cout << "\nSentence rejected.\n\n";
                }
                break;
            }
        case 6:
            {
                if (faNullFlag) {
                    std::cout << "\nNo FA loaded yet.\n\n";
                    break;
                }
                testMultipleSentences(fa);
                break;
            }
        default:
            quit = true;
            break;
        }
    }

    return 0;
}

/**
 * @brief Loads a FA from a file that contains a FA
 * 
 * @param faNullFlag Pointer to a flag that indicates if the FA is null
 * @return The FA loaded from the file
 */
FA loadDfaFromFile(bool* faNullFlag) {
    std::cout << "File name to load: ";
    std::string file_name;
    std::cin >> file_name;

    std::cout << "Loading file...\n";

    std::string s_base_path = BASE_PATH;
    std::string file_path = s_base_path + "Data/" + file_name;

    if (!existsFile(file_path)) {
        std::cout << "\nFile not found.\n\n";
        *faNullFlag = true;
        return FA();
    }

    pugi::xml_document file;
    pugi::xml_parse_result result = file.load_file(file_path.c_str());

    if (!result) {
        std::cout << "\nError loading file.\n\n";
        *faNullFlag = true;
        return FA();
    }
    
    std::cout << "File loaded successfully.\n";

    std::cout << "Setting up FA...\n";

    if (strcmp(file.child("structure").child_value("type"), "fa") != 0) {
        std::cout << "\nFile is not a finite automaton file.\n\n";
        *faNullFlag = true;
        return FA();
    }

    FA fa = FA();
    pugi::xml_node automaton = file.child("structure").child("automaton");

    // Setting up states
    for (pugi::xml_node node = automaton.child("state"); node; node = node.next_sibling("state")) {
        std::string id = node.attribute("id").value();
        fa.addState(id);
        if (node.child("initial")) {
            fa.setInitialState(id);
        }
        if (node.child("final")) {
            fa.addFinalState(id);
        }
    }

    // Setting up transitions
    for (pugi::xml_node node = automaton.child("transition"); node; node = node.next_sibling("transition")) {
        std::string symbol = node.child_value("read");
        symbol = treatStringChar(symbol);
        fa.addSymbol(symbol);
        fa.addTransition(node.child_value("from"), symbol, node.child_value("to"));
    }

    std::cout << "FA successfully setted up.\n\n";

    *faNullFlag = false;
    return fa;
}

/**
 * @brief Loads a FA from a file that contains a regular expression
 * 
 * @param faNullFlag Pointer to a flag that indicates if the FA is null
 * @return The FA loaded from the file
 */
FA loadDfaFromERFile(bool* faNullFlag) {
    std::cout << "File name to load: ";
    std::string file_name;
    std::cin >> file_name;

    std::cout << "\nLoading file...\n";

    std::string s_base_path = BASE_PATH;
    std::string file_path = s_base_path + "Data/" + file_name;

    if (!existsFile(file_path)) {
        std::cout << "\nFile not found.\n\n";
        *faNullFlag = true;
        return FA();
    }

    pugi::xml_document file;
    pugi::xml_parse_result result = file.load_file(file_path.c_str());

    if (!result) {
        std::cout << "\nError loading file.\n\n";
        *faNullFlag = true;
        return FA();
    }
    
    std::cout << "File loaded successfully.\n";

    std::cout << "Setting up FA...\n";

    if (strcmp(file.child("structure").child_value("type"), "re") != 0) {
        std::cout << "\nFile is not a regular expression file.\n\n";
        *faNullFlag = true;
        return FA();
    }

    std::string regular_expression = file.child("structure").child_value("expression");

    regular_expression = treatExpression(regular_expression);

    return loadFAFromRegularExpression(faNullFlag, regular_expression);
}

/**
 * @brief Treats an expression if it contains λ or spaces. Uses & instead of λ because the
 * unicode character is not recognized by the compiler
 * 
 * @param expression The expression to be treated
 * @return The treated expression
 */
std::string treatExpression(std::string expression) {
    std::string treatedExpression = "";

    // Getting λ
    for (int i = 0; i < expression.length(); i++) {
        char c = expression[i];
        if (((int) c) >= 0) {
            treatedExpression += c;
        } else {
            char next_char = expression[i + 1];
            if (next_char == '\0' || ((int) next_char) >= 0) {
                treatedExpression += '&';
            }
        }
    }

    // Removing spaces
    treatedExpression.erase(std::remove(treatedExpression.begin(), treatedExpression.end(), ' '), treatedExpression.end());

    return treatedExpression;
}

/**
 * @brief Treats a string that contains a character. If the character is not a unicode character,
 * it assumes it is λ and returns &. Otherwise, it returns the character
 * 
 * @param stringChar A string that contains a character
 * @return std::string The treated string
 */
std::string treatStringChar(std::string stringChar) {
    if (stringChar.length() > 1) return stringChar;
    if (stringChar.length() <= 0) return "&";
    char c = stringChar[0];
    if (((int) c) >= 0) {
        return stringChar;
    } else {
        return "&";
    }
}

/**
 * @brief Exports a FA to a file
 * 
 * @param fa The FA to be exported
 */
void exportDfaToFile(FA fa) {
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

    std::cout << "Exporting FA...\n";
    
    pugi::xml_node automaton = skeleton.child("structure").child("automaton");

    // Setting up states
    for (state s : fa.getStates()) {
        pugi::xml_node state = automaton.append_child("state");
        state.append_attribute("id") = s.c_str();
        state.append_attribute("name") = ("q" + s).c_str();
        state.append_child("x").append_child(pugi::node_pcdata).set_value("0");
        state.append_child("y").append_child(pugi::node_pcdata).set_value("0");
        if (s == fa.getInitialState()) {
            state.append_child("initial");
        }
        if (fa.isFinalState(s)) {
            state.append_child("final");
        }
    }

    // Setting up transitions
    for (std::pair<const transition, std::set<state>> const& it : fa.getTransitions()) {
        const pugi::char_t* state1 = it.first.first.c_str();
        const pugi::char_t* symbol = it.first.second.c_str();
        if (strcmp(symbol, "&") == 0) {
            symbol = "";
        }
        for (state s : it.second) {
            const pugi::char_t* state2 = s.c_str();
            pugi::xml_node transition = automaton.append_child("transition");
            transition.append_child("from").append_child(pugi::node_pcdata).set_value(state1);
            transition.append_child("to").append_child(pugi::node_pcdata).set_value(state2);
            transition.append_child("read").append_child(pugi::node_pcdata).set_value(symbol);
        }
    }

    skeleton.save_file(file_path.c_str());

    std::cout << "\nDFA successfully exported to " + file_path + ".\n\n";
}

/**
 * @brief Generates a Finite Automaton from a Regular Expression
 * 
 * @param faNullFlag A flag that indicates if the FA is null
 * @return The generated FA
 */
FA loadFAFromRegularExpression(bool* faNullFlag, std::string regular_expression) {
    std::cout << "Loading FA from regular expression...\n";

    FA fa = getFAFromRE(regular_expression);

    if (fa.getStates().size() == 0) {
        std::cout << "\nInvalid regular expression.\n\n";
        *faNullFlag = true;
        return FA();
    }

    std::cout << "FA successfully loaded.\n\n";

    *faNullFlag = false;
    return fa;
}

/**
 * @brief Transforms a NFA to a DFA
 * 
 * @param fa The NFA to be transformed
 * @return The transformed DFA
 */
FA transformNfaToDfa(FA fa) {
    FA newFA = fa;
    if (newFA.hasLambda()) {
        std::cout << "\nRemoving lambda transitions...\n";
        newFA = removeLambdaTransitions(newFA);
        std::cout << "Lambda transitions successfully removed.\n";
    }
    if (!newFA.isDeterministic()) {
        std::cout << "\nDeterminizing...\n";
        newFA = determinizeFA(newFA);
        newFA.removeUnreachableStates();
        std::cout << "FA successfully determinized.\n";
    }
    std::cout << "\n";
    return newFA;
}

/**
 * @brief Runs an O(n^2) algorithm that minimizes a DFA
 * 
 * @param dfa The DFA to be minimized
 * 
 * @return The minimized DFA
 */
FA minimizeDFA(FA dfa) {
    if (dfa.hasLambda() || !dfa.isDeterministic()) {
        std::cout << "\nThe automaton is not deterministic.\n\n";
        return dfa;
    }
    return automatonMinimizationAlgorithm(dfa);
}

/**
 * @brief Generates a DFA with n states. The DFA forces the worst case to the O(n^2) algorithm.
 * 
 * @param n The number of states
 * @return The generated DFA
 */
FA generateDfa(int n) {
    FA dfa = FA();
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

/**
 * @brief Tests multiple sentences in a FA. The sentences are read from a file.
 * 
 * @param fa The FA to be tested
 */
void testMultipleSentences(FA fa) {
    std::cout << "File name to load: ";
    std::string file_name;
    std::cin >> file_name;

    std::cout << "Loading file...\n";

    std::string s_base_path = BASE_PATH;
    std::string file_path = s_base_path + "Data/" + file_name;

    if (!existsFile(file_path)) {
        std::cout << "\nFile not found.\n\n";
        return;
    }

    std::ifstream file(file_path);
    std::string line;

    if (file.is_open()) {
        while (getline(file,line)) {
            std::cout << "\nSentence: " << line << " => ";
            if (fa.testSentence(line)) {
                std::cout << "Accepted.";
            } else {
                std::cout << "Rejected.";
            }
        }
        file.close();
    }

    std::cout << "\n\n";
}