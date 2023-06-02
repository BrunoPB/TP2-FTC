/**
 * @author Bruno Pena Baêta (696997)
 * @author Felipe Nepomuceno Coelho (689661)
 */

#include <set>
#include <map>
#include <utility>
#include <iostream>
#include "utils.cpp"

typedef std::string state;
typedef std::pair<state, std::string> transition;

/**
 * @brief A class representing a Finite Automaton
 */
class FA {
private:
    std::set<state> states;
    std::set<std::string> alphabet;
    std::map<transition, std::set<state>> transitions;
    state initial_state;
    std::set<state> final_states;

public:
    // Constructors
    FA() {
        this->states = std::set<state>();
        this->alphabet = std::set<std::string>();
        this->transitions = std::map<transition, std::set<state>>();
        this->initial_state = "";
        this->final_states = std::set<state>();
    }

    FA(std::set<state> states,
        std::set<std::string> alphabet,
        std::map<transition, std::set<state>> transitions,
        state initial_state,
        std::set<state> final_states) {
        this->states = states;
        this->alphabet = alphabet;
        this->transitions = transitions;
        this->initial_state = initial_state;
        this->final_states = final_states;
    }

    // FA Creation
    /**
     * @brief Adds a state to the FA
     * 
     * @param s The state to be added
     */
    void addState(state s) {
        this->states.insert(s);
    }

    /**
     * @brief Adds a symbol to the FA's alphabet
     * 
     * @param symbol The symbol to be added
     */
    void addSymbol(std::string symbol) {
        if (symbol.length() == 0) {
            std::cout << "Symbol cannot be empty" << std::endl;
            return;
        }
        if (symbol.length() <= 1) {
            char c = symbol[0];
            if (((int) c) <= 0) {
                std::cout << "The symbol " << c << " can not be added to the alphabet."<< std::endl;
                return;
            }
            if (c == '&') {
                std::cout << "The symbol & can not be added to the alphabet."<< std::endl;
                return;
            }
        }
        this->alphabet.insert(symbol);
    }

    /**
     * @brief Adds a transition to the FA
     * 
     * @param from The state from which the transition starts
     * @param read The symbol that triggers the transition
     * @param to The state to which the transition goes
     */
    void addTransition(state from, std::string read, state to) {
        this->transitions[std::make_pair(from, read)].insert(to);
    }

    /**
     * @brief Sets the FA's initial state
     * 
     * @param s The state to be set as initial
     */
    void setInitialState(state s) {
        this->initial_state = s;
    }

    /**
     * @brief Adds a final state to the FA
     * 
     * @param s The state to be added as final
     */
    void addFinalState(state s) {
        this->final_states.insert(s);
    }

    // FA Information
    /**
     * @brief Checks if a state is a final state
     * 
     * @param s The state to be checked
     * @return true if the state is a final state. false otherwise
     */
    bool isFinalState(state s) {
        return this->final_states.find(s) != this->final_states.end();
    }

    /**
     * @brief Gets the FA's initial state
     * 
     * @return The FA's initial state
     */
    state getInitialState() {
        return this->initial_state;
    }

    /**
     * @brief Gets a set of the FA's final states
     * 
     * @return The FA's final states
     */
    std::set<state> getFinalStates() {
        return this->final_states;
    }

    /**
     * @brief Gets a set of the FA's non-final states
     * 
     * @return The FA's non-final states
     */
    std::set<state> getNonFinalStates() {
        std::set<state> non_final_states;
        for (state s : this->states) {
            if (!this->isFinalState(s)) {
                non_final_states.insert(s);
            }
        }
        return non_final_states;
    }

    /**
     * @brief Clears the FA's final states
     * 
     */
    void clearFinalStates() {
        this->final_states.clear();
    }

    /**
     * @brief Prints the SuperFA's states to the console
     * 
     */
    void printStates() {
        std::string text = "States: {";
        for (state st : this->states) {
            text += (st + ",");
        }
        text.pop_back();
        text += "}";
        std::cout << text << std::endl;
    }

    /**
     * @brief Gets the state to which a transition goes when a symbol is read by a state
     * 
     * @param from The state from which the transition starts
     * @param read The symbol that triggers the transition
     * @return The set of states to which the transition goes
     */
    std::set<state> transite(state from, std::string read) {
        return this->transitions[std::make_pair(from, read)];
    }

    /**
     * @brief Gets all the states of the FA
     * 
     * @return A set of states that contains all the states of the FA
     */
    std::set<state> getStates() {
        return this->states;
    }

    /**
     * @brief Gets all the transitions of the FA
     * 
     * @return A map that contains all the transitions of the FA
     */
    std::map<transition, std::set<state>> getTransitions() {
        return this->transitions;
    }

    /**
     * @brief Gets all the symbols of the FA's alphabet
     * 
     * @return A set of strings that contains all the symbols of the FA's alphabet
     */
    std::set<std::string> getAlphabet() {
        return this->alphabet;
    }

    /**
     * @brief Checks if the FA is deterministic
     * 
     * @return true if the FA is deterministic. false otherwise
     */
    bool isDeterministic() {
        if (this->hasLambda()) return false;
        for (state s : this->states) {
            for (std::string symbol : this->alphabet) {
                if (this->transite(s,symbol).size() > 1) {
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
        for (auto transition : this->transitions) {
            if (transition.first.second.length() > 1) {
                continue;
            }
            char c = transition.first.second[0];
            if (((int) c) <= 0 || c == '&' || c == 'λ') {
                return true;
            }
        }
        return false;
    }

    // FA Operations
    /**
     * @brief Removes all the unreachable states from the FA
     */
    void removeUnreachableStates() {
        if (this->hasLambda()) {
            std::cout << "The FA has lambda transitions. It is not possible to remove unreachable states yet." << std::endl;
            return;
        }
        
        if (!this->isDeterministic()) {
            std::cout << "The FA is not deterministic. It is not possible to remove unreachable states yet." << std::endl;
            return;
        }

        std::set<state> reachable_states;
        std::set<state> new_reachable_states;
        reachable_states.insert(this->initial_state);
        new_reachable_states.insert(this->initial_state);
        while (new_reachable_states.size() > 0) {
            std::set<state> aux;
            for (state s : new_reachable_states) {
                for (std::string symbol : this->alphabet) {
                    state next_state = *this->transite(s, symbol).begin();
                    if (!has(reachable_states,next_state)) {
                        aux.insert(next_state);
                    }
                }
            }
            new_reachable_states = aux;
            reachable_states.insert(new_reachable_states.begin(), new_reachable_states.end());
        }
        std::set<state> unreachable_states;
        for (state s : this->states) {
            if (!has(reachable_states, s)) {
                unreachable_states.insert(s);
            }
        }
        for (state s : unreachable_states) {
            this->states.erase(s);
            for (std::string symbol : this->alphabet) {
                this->transitions.erase(std::make_pair(s, symbol));
            }
        }
    }

    /**
     * @brief Renames a single state of the FA
     * 
     * @param s The state to be renamed
     * @param new_name The new name of the state
     */
    void renameState(state s, std::string new_name) {
        // Renaming state
        this->states.erase(s);
        this->addState(new_name);

        std::cout << "RENAMING STATE " << s << " TO " << new_name << std::endl;

        // Updating transitions
        for (auto const& aTransition : this->getTransitions()) {
            if (aTransition.second.size() == 0) continue; // No transition
            std::string to_state_name = *aTransition.second.begin();

            std::cout << "Checking transition " << aTransition.first.first << " -> " << aTransition.first.second << " -> " << to_state_name << std::endl;

            // If the transition is from the state to itself, that transition must be updated in a special way
            if (strcmp(aTransition.first.first.c_str(),s.c_str()) == 0  && has(aTransition.second, s) && strcmp(to_state_name.c_str(),aTransition.first.first.c_str()) == 0) {
                std::cout << "Transition is from state to itself" << std::endl;                
                this->transitions.erase(aTransition.first);
                this->transitions[std::make_pair(new_name, aTransition.first.second)].insert(new_name);
                continue;
            }
            // Has transition to it
            if (strcmp(to_state_name.c_str(),s.c_str()) == 0) {
                std::cout << "Transition is to it" << std::endl;
                this->transitions[aTransition.first].insert(new_name);
                this->transitions[aTransition.first].erase(s);
            }
            // Has transition from it
            if (strcmp(aTransition.first.first.c_str(),s.c_str()) == 0) {
                std::cout << "Transition is from it" << std::endl;
                this->transitions[std::make_pair(new_name, aTransition.first.second)] = aTransition.second;
                this->transitions.erase(aTransition.first);
            }

            std::cout << "New transition " << aTransition.first.first << " -> " << aTransition.first.second << " -> " << to_state_name << std::endl;
        }

        // Updating initial state
        if (this->initial_state == s) {
            this->setInitialState(new_name);
        }

        // Updating final states
        if (this->isFinalState(s)) {
            this->final_states.erase(s);
            this->addFinalState(new_name);
        }

        std::cout << "===========================================================" << std::endl;
    }

    /**
     * @brief Renames all the states of the FA
     * 
     */
    void renameAutomatonStates() {
        int state_value = 0;
        std::set<state> theStates = this->getStates();
        for (state s : theStates) {
            while (has(theStates,std::to_string(state_value))) {
                state_value++;
            }
            this->renameState(s, std::to_string(state_value));
            state_value++;
        }
    }

    /**
     * @brief Complete the FA
     */
    void completeAutomaton() {
        if (this->hasLambda()) {
            std::cout << "The automaton has lambda transitions." << std::endl;
            return;
        }

        if (!this->isDeterministic()) {
            std::cout << "The automaton is not deterministic." << std::endl;
            return;
        }

        int error_state_value = 0;
        for (state s : this->getStates()) {
            try {
                if (std::stoi(s) > error_state_value) {
                    error_state_value = std::stoi(s);
                }
            } catch (std::exception& e) {
                continue;
            }
        }
        error_state_value++;
        state error_state = std::to_string(error_state_value);
        this->addState(error_state);
        for (state s : this->getStates()) {
            for (std::string symbol : this->getAlphabet()) {
                if (this->transitions.find(std::make_pair(s, symbol)) == this->transitions.end()) {
                    this->addTransition(s, symbol, error_state);
                }
            }
        }
    }

    /**
     * @brief Tests if a sentence is accepted by the FA
     * 
     * @param sentence The sentence to be tested
     * @return true if the sentence is accepted by the FA. false otherwise
     */
    bool testSentence(std::string sentence, state current_state = "-") {
        if (strcmp(current_state.c_str(),"-")) {
            current_state = this->initial_state;
        }

        if (sentence.empty()) {
            return this->isFinalState(current_state);
        }

        std::string symbol(1, sentence[0]);
        if (this->transite(current_state, symbol).empty()) {
            return false;
        }

        if (this->transite(current_state, "&").size() > 0) {
            for (state s : this->transite(current_state, "&")) {
                if (this->testSentence(sentence, s)) {
                    return true;
                }
            }
        }

        for (state s : this->transite(current_state, symbol)) {
            if (sentence.length() == 1) {
                if (this->testSentence("", s)) {
                    return true;
                }
            } else {
                if (this->testSentence(sentence.substr(1), s)) {
                    return true;
                }
            }
        }

        return false;
    }

    /**
     * @brief Get all the states that can be reached from a given state by lambda transitions
     * 
     * @param starting_state The state from which the lambda transitions start
     * @return A set of states that can be reached from the starting state by lambda transitions
     */
    std::set<state> getStatesFromLambdaTransition(state starting_state) {
        std::set<state> states_from_lambda_transition;
        std::set<state> new_states_from_lambda_transition;
        states_from_lambda_transition.insert(starting_state);
        new_states_from_lambda_transition.insert(starting_state);
        while (new_states_from_lambda_transition.size() > 0) {
            std::set<state> aux;
            for (state s : new_states_from_lambda_transition) {
                for (state next_state : this->transite(s, "&")) {
                    if (!has(states_from_lambda_transition, next_state)) {
                        aux.insert(next_state);
                    }
                }
            }
            new_states_from_lambda_transition = aux;
            states_from_lambda_transition.insert(new_states_from_lambda_transition.begin(), new_states_from_lambda_transition.end());
        }
        return states_from_lambda_transition;
    }
};