/**
 * @author Bruno Pena Baêta (696997)
 * @author Felipe Nepomuceno Coelho (689661)
 */

#include <set>
#include <map>
#include <utility>
#include <iostream>
#include "fa.cpp"

typedef std::set<state> superState;
typedef std::pair<std::set<state>, std::string> superTransition;

/**
 * @brief A class representing what we called a Super Finite Automaton. Basically, it is a
 * SuperFA that can have a set of states as states, what we called super states.
 */
class SuperFA {
private:
    std::set<superState> states;
    std::set<std::string> alphabet;
    std::map<superTransition, superState> transitions;
    superState initial_state;
    std::set<superState> final_states;

public:
    // Constructors
    SuperFA() {
        this->states = std::set<superState>();
        this->alphabet = std::set<std::string>();
        this->transitions = std::map<superTransition, superState>();
        this->initial_state = superState();
        this->final_states = std::set<superState>();
    }

    SuperFA(std::set<superState> states,
        std::set<std::string> alphabet,
        std::map<superTransition, superState> transitions,
        superState initial_state,
        std::set<superState> final_states) {
        this->states = states;
        this->alphabet = alphabet;
        this->transitions = transitions;
        this->initial_state = initial_state;
        this->final_states = final_states;
    }

    // SuperFA Creation
    /**
     * @brief Adds a super state to the SuperFA
     * 
     * @param s The super state to be added
     */
    void addState(superState s) {
        if (s.size() == 0) {
            return;
        }
        if (has(this->getStates(),s)) {
            return;
        }
        this->states.insert(s);
    }

    /**
     * @brief Adds a symbol to the SuperFA's alphabet
     * 
     * @param symbol The symbol to be added
     */
    void addSymbol(std::string symbol) {
        if (symbol.length() == 0) {
            return;
        }
        if (symbol.length() <= 1) {
            char c = symbol[0];
            if (((int) c) <= 0) {
                return;
            }
            if (c == '&') {
                return;
            }
        }
        this->alphabet.insert(symbol);
    }

    /**
     * @brief Adds a super transition to the SuperFA
     * 
     * @param from The super state from which the super transition starts
     * @param read The symbol that triggers the super transition
     * @param to The super state to which the transition goes
     */
    void addTransition(superState from, std::string read, superState to) {
        this->transitions[std::make_pair(from, read)] = to;
    }

    /**
     * @brief Sets the SuperFA's initial super state
     * 
     * @param s The super state to be set as initial
     */
    void setInitialState(superState s) {
        this->initial_state = s;
    }

    /**
     * @brief Adds a final super state to the SuperFA
     * 
     * @param s The super state to be added as final
     */
    void addFinalState(superState s) {
        this->final_states.insert(s);
    }

    // SuperFA Information
    /**
     * @brief Checks if a super state is a final super state
     * 
     * @param s The super state to be checked
     * @return true if the super state is a final super state. false otherwise
     */
    bool isFinalState(superState s) {
        return this->final_states.find(s) != this->final_states.end();
    }

    /**
     * @brief Gets the SuperFA's initial super state
     * 
     * @return The SuperFA's initial super state
     */
    superState getInitialState() {
        return this->initial_state;
    }

    /**
     * @brief Gets a set of the SuperFA's final super states
     * 
     * @return The SuperFA's final states
     */
    std::set<superState> getFinalStates() {
        return this->final_states;
    }

    /**
     * @brief Gets a set of the SuperFA's non-final super states
     * 
     * @return The SuperFA's non-final super states
     */
    std::set<superState> getNonFinalStates() {
        std::set<superState> non_final_states;
        for (superState s : this->states) {
            if (!this->isFinalState(s)) {
                non_final_states.insert(s);
            }
        }
        return non_final_states;
    }

    /**
     * @brief Gets the super state to which a super transition goes when a symbol is read by a super state
     * 
     * @param from The super state from which the super transition starts
     * @param read The symbol that triggers the super transition
     * @return The super state to which the super transition goes
     */
    superState transite(superState from, std::string read) {
        return this->transitions[std::make_pair(from, read)];
    }

    /**
     * @brief Gets all the super states of the SuperFA
     * 
     * @return A set of super states that contains all the super states of the SuperFA
     */
    std::set<superState> getStates() {
        return this->states;
    }

    /**
     * @brief Gets all the super transitions of the SuperFA
     * 
     * @return A map that contains all the transitions of the SuperFA
     */
    std::map<superTransition, superState> getTransitions() {
        return this->transitions;
    }

    /**
     * @brief Gets all the symbols of the SuperFA's alphabet
     * 
     * @return A set of strings that contains all the symbols of the SuperFA's alphabet
     */
    std::set<std::string> getAlphabet() {
        return this->alphabet;
    }

    /**
     * @brief Prints the SuperFA's states to the console
     * 
     */
    void printStates() {
        std::string text = "States: {";
        for (superState s : this->states) {
            text += "{";
            for (state st : s) {
                text += (st + ",");
            }
            text.pop_back();
            text += "},";
        }
        text.pop_back();
        text += "}";
        std::cout << text << std::endl;
    }

    /**
     * @brief Checks if the SuperFA is deterministic
     * 
     * @return true if the SuperFA is deterministic. false otherwise
     */
    bool isDeterministic() {
        for (superState s : this->states) {
            for (std::string symbol : this->alphabet) {
                if (this->transite(s, symbol).size() > 1) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * @brief Checks if the FA has lambda transitions
     * 
     * @return true if the FA has lambda transitions. false otherwise
     */
    bool hasLambda() {
        for (superState s : this->states) {
            if (this->transite(s, "").size() > 0) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Convert the SuperFA to a FA
     * 
     * @return The FA that is equivalent to the SuperFA
     */
    FA convertToFa() {
        FA fa = FA();
        std::string separator = ",";

        // Setting up alphabet
        for (std::string symbol : this->alphabet) {
            fa.addSymbol(symbol);
        }

        // Setting up states
        for (superState ss : this->getStates()) {
            std::string state_name = "";
            for (state s : ss) {
                state_name += (s + separator);
            }
            if (state_name == "") continue;
            state_name.pop_back();
            fa.addState(state_name);
        }

        // Setting up initial state
        std::string initial_state_name = "";
        for (state s : this->initial_state) {
            initial_state_name += (s + separator);
        }
        initial_state_name.pop_back();
        fa.setInitialState(initial_state_name);

        // Setting up final states
        for (superState super_state : this->final_states) {
            std::string state_name = "";
            for (state s : super_state) {
                state_name += (s + separator);
            }
            state_name.pop_back();
            fa.addFinalState(state_name);
        }

        // Setting up transitions
        for (std::pair<superTransition, superState> p : this->transitions) {
            // Getting from state name
            if (p.first.first.size() == 0) {
                continue;
            }
            std::string from_state_name = "";
            for (state s : p.first.first) {
                from_state_name += (s + separator);
            }
            from_state_name.pop_back();

            // Getting symbol
            if (p.first.second.size() == 0) {
                continue;
            }
            std::string symbol = p.first.second;

            // Getting to state name
            if (p.second.size() == 0) {
                continue;
            }
            std::string to_state_name = "";
            superState to_super_state = p.second;
            for (state s : to_super_state) {
                to_state_name += (s + separator);
            }
            to_state_name.pop_back();

            fa.addTransition(from_state_name, symbol, to_state_name);
        }

        return fa;
    }
};