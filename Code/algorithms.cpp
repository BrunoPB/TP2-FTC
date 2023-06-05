/**
 * @author Bruno Pena Baêta (696997)
 * @author Felipe Nepomuceno Coelho (689661)
 */

#include "superFa.cpp"

/**
 * @brief Checks if two states are equivalent in a set Q
 * 
 * @param s1 The first state
 * @param s2 The second state
 * @param dfa The DFA the states are in
 * @param Q The set of super states
 * @return true if the states are equivalent. false otherwise
 */
bool areEquivalent(state s1, state s2, FA dfa, std::set<superState> Q) {
    if (dfa.isFinalState(s1) && !dfa.isFinalState(s2)) {
        return false;
    } else if (!dfa.isFinalState(s1) && dfa.isFinalState(s2)) {
        return false;
    } else {
        for (std::string symbol : dfa.getAlphabet()) {
            state transitionState1 = *dfa.transite(s1,symbol).begin();
            state transitionState2 = *dfa.transite(s2,symbol).begin();
            // Checking if the transition states are in the same super state of Q
            for (superState ss : Q) {
                if (has(ss,transitionState1) && !has(ss,transitionState2)) {
                    return false;
                } else if (!has(ss,transitionState1) && has(ss,transitionState2)) {
                    return false;
                } else if (has(ss,transitionState1) && has(ss,transitionState2)) {
                    return true;
                }
            }
        }
    }
    return false;
}

/**
 * @brief Runs an O(n^2) algorithm that minimizes a DFA.
 * 
 * @param dfa The DFA to be minimized
 * @return The minimized DFA
 */
FA automatonMinimizationAlgorithm(FA dfa) {
    // Checking if the FA is a DFA
    if (dfa.hasLambda()) {
        std::cout << "The FA has lambda transitions. Cannot minimize.\n";
        return dfa;
    }

    if (!dfa.isDeterministic()) {
        std::cout << "The FA is not a DFA. Cannot minimize.\n";
        return dfa;
    }

    // Initialization
    std::cout << "Preparing to run the O(n^2) algorithm...\n";

    std::cout << "Removing unreachable states...\n";
    dfa.removeUnreachableStates();
    std::cout << "Unreachable states successfully removed.\n";

    std::cout << "Running the O(n^2) algorithm...\n";

    // Algorithm
    int equivalence = 0;
    std::set<superState> Q[1024];
    Q[equivalence] = std::set<superState>();
    Q[equivalence].insert(dfa.getNonFinalStates());
    Q[equivalence].insert(dfa.getFinalStates());
    do {
        equivalence++;
        Q[equivalence] = std::set<superState>();
        Q[equivalence].clear();
        for(superState ss : Q[equivalence-1]) {
            // Ignore empty sets
            if (ss.size() == 0) {
                continue;
            }
            // Sets with 1 element
            if (ss.size() == 1) {
                Q[equivalence].insert(ss);
            }
            // Sets with 2 elements or more
            for (int i = 0; i < ss.size()-1; i++) {
                state compare1 = *std::next(ss.begin(), i);
                state compare2 = *std::next(ss.begin(), i+1);
                if (areEquivalent(compare1,compare2,dfa,Q[equivalence-1])) {
                    // Put in same set
                    bool had = false; // Indicates if there was already a set with that element
                    for (superState sss : Q[equivalence]) {
                        if (has(sss,compare1)) {
                            superState newSuperState = sss;
                            Q[equivalence].erase(sss);
                            newSuperState.insert(compare2);
                            Q[equivalence].insert(newSuperState);
                            had = true;
                            break;
                        }
                    }
                    if (!had) {
                        superState newSuperState = superState();
                        newSuperState.insert(compare1);
                        newSuperState.insert(compare2);
                        Q[equivalence].insert(newSuperState);
                    }
                } else {
                    // Put in different sets
                    bool had = false; // Indicates if there was already a set with that element
                    for (superState sss : Q[equivalence]) {
                        if (has(sss,compare1)) {
                            had = true;
                            break;
                        }
                    }
                    if (had) {
                        superState newSuperState = superState();
                        newSuperState.insert(compare2);
                        Q[equivalence].insert(newSuperState);
                    } else {
                        superState newSuperState = superState();
                        superState newSuperState2 = superState();
                        newSuperState.insert(compare1);
                        newSuperState2.insert(compare2);
                        Q[equivalence].insert(newSuperState);
                        Q[equivalence].insert(newSuperState2);
                    }
                }
            }
        }
    } while (Q[equivalence].size() != Q[equivalence-1].size());
    

    // Creating the new FA from a SuperFA
    SuperFA superDfa = SuperFA();

    // Setting up alphabet (Sigma')
    for (std::string symbol : dfa.getAlphabet()) {
        superDfa.addSymbol(symbol);
    }

    // Setting up states (Q')
    for (superState ss : Q[equivalence]) {
        superDfa.addState(ss);
    }

    // Setting up initial state (i')
    for (auto ss : superDfa.getStates()) {
        if (has(ss,dfa.getInitialState())) {
            superDfa.setInitialState(ss);
            break;
        }
    }

    // Setting up final states (F')
    for (auto ss : superDfa.getStates()) {
        for (state s : dfa.getFinalStates()) {
            if (has(ss,s)) {
                superDfa.addFinalState(ss);
            }
        }
    }

    // Setting up transitions (delta')
    for (superState X : Q[equivalence]) {
        for (std::string a : superDfa.getAlphabet()) {
            for (state e : X) {
                state transitionState = *dfa.transite(e,a).begin();
                for (superState Y : Q[equivalence]) {
                    if (has(Y,transitionState)) {
                        superDfa.addTransition(X,a,Y);
                    }
                }
            }
        }
    }

    std::cout << "O(n^2) algorithm successfully executed.\n";

    std::cout << "Minimizing the FA...\n";

    // Converting the SuperFA to a FA
    FA newDfa = superDfa.convertToFa();

    std::cout << "FA successfully minimized!\n\n";

    return newDfa;
}

/**
 * @brief Concatenates 2 automatons and create a single NFA with the 2 automatons
 * 
 * @param fa1 The starting FA
 * @param fa2 The ending FA
 * @param state_name_differ A pointer to an int that will be used to make the state names differ
 * @return The new NFA
 */
FA concatAutomatons(FA fa1, FA fa2, int* state_name_differ) {
    FA newFa = fa1;
    std::string prefix = std::to_string(*state_name_differ);
    std::string separator = "c";

    // Adding the alphabet of fa2 to fa1
    for (std::string symbol : fa2.getAlphabet()) {
        newFa.addSymbol(symbol);
    }

    // Adding the states of fa2 to fa1
    for (state s : fa2.getStates()) {
        state stateName = prefix + separator + s;
        newFa.addState(stateName);
    }

    // Adding the transitions of fa2 to fa1
    for (auto const& xTransition : fa2.getTransitions()) {
        for (state s : xTransition.second) {
            state to_state = prefix + separator + s;
            state from_state = prefix + separator + xTransition.first.first;
            std::string symbol = xTransition.first.second;
            newFa.addTransition(from_state,symbol,to_state);
        }
    }

    // Connecting fa1 final states to fa2 initial state
    for (state s : fa1.getFinalStates()) {
        state stateName = prefix + separator + fa2.getInitialState();
        newFa.addTransition(s,"&",stateName);
    }

    // Removing fa1 final states
    newFa.clearFinalStates();

    // Adding fa2 final states
    for (state s : fa2.getFinalStates()) {
        state stateName = prefix + separator + s;
        newFa.addFinalState(stateName);
    }

    return newFa;
}


/**
 * @brief Creates a Kleene star of a FA
 * 
 * @param fa The FA to be transformed
 * @param state_name_differ A pointer to an int that will be used to make the state names differ
 * @return The transformed FA
 */
FA kleeneStar(FA fa, int* state_name_differ) {
    FA newFa = fa;
    std::string prefix = std::to_string(*state_name_differ);
    std::string initial_separator = "i";
    std::string final_separator = "f";

    // Connecting final states to initial state
    for (state s : fa.getFinalStates()) {
        newFa.addTransition(s,"&",fa.getInitialState());
    }

    // Creating new initial state
    state newInitialState = prefix + initial_separator + fa.getInitialState();
    newFa.addState(newInitialState);
    newFa.setInitialState(newInitialState);

    // Connecting new initial state to old initial state
    newFa.addTransition(newInitialState,"&",fa.getInitialState());

    // Creating new final state
    state newFinalState = prefix + final_separator + *fa.getFinalStates().begin();
    newFa.addState(newFinalState);

    // Connecting old final states to new final state
    for (state s : fa.getFinalStates()) {
        newFa.addTransition(s,"&",newFinalState);
    }
    newFa.clearFinalStates();
    newFa.addFinalState(newFinalState);

    // Connecting new initial state to new final state
    newFa.addTransition(newInitialState,"&",newFinalState);

    return newFa;
}


/**
 * @brief Creates a union of 2 automatons (U or + or |)
 * 
 * @param fa1 The first FA
 * @param fa2 The second FA
 * @param state_name_differ A pointer to an int that will be used to make the state names differ
 * @return The new FA
 */
FA uniteAutomatons(FA fa1, FA fa2, int* state_name_differ) {
    FA newFa = FA();
    std::string prefix = std::to_string(*state_name_differ);
    std::string separator1 = "u";
    std::string separator2 = "s";

    // Adding the alphabet of fa1 and fa2 to the new FA
    for (std::string symbol : fa1.getAlphabet()) {
        newFa.addSymbol(symbol);
    }
    for (std::string symbol : fa2.getAlphabet()) {
        newFa.addSymbol(symbol);
    }

    // Adding the states of fa1 and fa2 to the new FA
    for (state s : fa1.getStates()) {
        state stateName = prefix + separator1 + s;
        newFa.addState(stateName);
    }
    for (state s : fa2.getStates()) {
        state stateName = prefix + separator2 + s;
        newFa.addState(stateName);
    }

    // Adding the transitions of fa1 and fa2 to the new FA
    for (auto const& xTransition : fa1.getTransitions()) {
        for (state s : xTransition.second) {
            state to_state = prefix + separator1 + s;
            state from_state = prefix + separator1 + xTransition.first.first;
            std::string symbol = xTransition.first.second;
            newFa.addTransition(from_state,symbol,to_state);
        }
    }
    for (auto const& xTransition : fa2.getTransitions()) {
        for (state s : xTransition.second) {
            state to_state = prefix + separator2 + s;
            state from_state = prefix + separator2 + xTransition.first.first;
            std::string symbol = xTransition.first.second;
            newFa.addTransition(from_state,symbol,to_state);
        }
    }

    // Creating new initial state
    state newInitialState = prefix + separator2 + fa1.getInitialState();
    newFa.addState(newInitialState);
    newFa.setInitialState(newInitialState);

    // Connecting new initial state to old initial states
    newFa.addTransition(newInitialState,"&",prefix + separator1 + fa1.getInitialState());
    newFa.addTransition(newInitialState,"&",prefix + separator2 + fa2.getInitialState());

    // Adding new final state
    state newFinalState = prefix + separator2 + *fa1.getFinalStates().begin();
    newFa.addState(newFinalState);
    newFa.addFinalState(newFinalState);

    // Connecting old final states to new final state
    for (state s : fa1.getFinalStates()) {
        newFa.addTransition(prefix + separator1 + s,"&",newFinalState);
    }
    for (state s : fa2.getFinalStates()) {
        newFa.addTransition(prefix + separator2 + s,"&",newFinalState);
    }

    return newFa;
}

/**
 * @brief Creates a single character FA
 * 
 * @param c The character
 * @param state_name_differ A pointer to an int that will be used to make the state names differ
 * @return The FA
 */
FA singleCharAutomaton(std::string c, int* state_name_differ) {
    FA fa = FA();
    std::string prefix = std::to_string(*state_name_differ);
    std::string separator = "a";
    std::string initial_state_name = prefix + separator + "0";
    std::string final_state_name = prefix + separator + "1";
    fa.addState(initial_state_name);
    fa.addState(final_state_name);
    fa.setInitialState(initial_state_name);
    fa.addFinalState(final_state_name);
    fa.addSymbol(c);
    fa.addTransition(initial_state_name,c, final_state_name);
    return fa;
}

/**
 * @brief Turns a sub-expression of a RE into a FA
 * 
 * @param re The complete RE
 * @param reading_index The index of the RE where the sub-expression starts
 * @param state_name_differ A pointer to an integer that will be used to make the names of the states of the FA unique
 * @return The generated FA
 */
FA getSubExFA(std::string re, FA* automaton, int* reading_index, int* state_name_differ) {
    if (*reading_index > re.size()-1) {
        return *automaton;
    }
    char c = re[*reading_index];
    
    switch (c) {
    case '+':
        {
            *reading_index = *reading_index + 1;
            FA sub_automaton = FA(
                {std::to_string(*state_name_differ)},
                std::set<std::string>(),
                std::map<transition, std::set<state>>(),
                std::to_string(*state_name_differ),
                {std::to_string(*state_name_differ)}
            );
            *state_name_differ = *state_name_differ + 1;
            sub_automaton = getSubExFA(re,&sub_automaton,reading_index,state_name_differ);
            *state_name_differ = *state_name_differ + 1;
            *automaton = uniteAutomatons(*automaton,sub_automaton,state_name_differ);
            *state_name_differ = *state_name_differ + 1;
            break;
        }
    case ')':
        *reading_index = *reading_index + 1;
        return *automaton;
        break;
    case '(':
        {
            *reading_index = *reading_index + 1;
            FA sub_automaton = FA(
                {"0"},
                std::set<std::string>(),
                std::map<transition, std::set<state>>(),
                "0",
                {"0"}
            );
            sub_automaton = getSubExFA(re,&sub_automaton,reading_index,state_name_differ);
            *state_name_differ = *state_name_differ + 1;

            // Kleene star
            if (re[*reading_index] == '*') {
                *reading_index = *reading_index + 1;
                sub_automaton = kleeneStar(sub_automaton,state_name_differ);
                *state_name_differ = *state_name_differ + 1;
            }

            *automaton = concatAutomatons(*automaton,sub_automaton,state_name_differ);
            *state_name_differ = *state_name_differ + 1;
            break;
        }
    default:
        {
            automaton->addSymbol(std::string(1,c));

            std::string symbol_string = std::string(1,c);

            FA sub_automaton = singleCharAutomaton(symbol_string,state_name_differ);
            *state_name_differ = *state_name_differ + 1;

            // Kleene star
            if (re[*reading_index+1] == '*') {
                *reading_index = *reading_index + 1;
                sub_automaton = kleeneStar(sub_automaton,state_name_differ);
                *state_name_differ = *state_name_differ + 1;
            }

            *automaton = concatAutomatons(*automaton, sub_automaton, state_name_differ);
            *state_name_differ = *state_name_differ + 1;

            break;
        }
    }
    *reading_index = *reading_index + 1;
    return getSubExFA(re, automaton, reading_index, state_name_differ);
}

/**
 * @brief Generates a Finite Automaton based on a Regular Expression
 * 
 * @param re The Regular Expression
 * @return A Finite Automaton based on the Regular Expression
 */
FA getFAFromRE(std::string re) {
    int state_name_differ = 0;
    int reading_index = 0;
    
    // Invalid RE
    if (re[0] == ')' || re[0] == '*' || re[0] == '+') return FA();

    FA automaton = FA(
        {"0"},
        std::set<std::string>(),
        std::map<transition, std::set<state>>(),
        "0",
        {"0"}
    );
    automaton = getSubExFA(re, &automaton, &reading_index, &state_name_differ);

    automaton.renameAutomatonStates();

    return automaton;
}

/**
 * @brief Removes all lambda transitions from a FA
 * 
 * @param fa The FA to be transformed
 * @return The transformed FA
 */
FA removeLambdaTransitions(FA fa) {
    if (!fa.hasLambda()) {
        return fa;
    }

    // Step 1
    // Get all fechos lambda
    std::set<std::pair<state,std::set<state>>> fechos_lambda = std::set<std::pair<state,std::set<state>>>();
    for (auto state : fa.getStates()) {
        fechos_lambda.insert(std::make_pair(state,fa.getStatesFromLambdaTransition(state)));
    }
    
    // Step 2
    // Creating δ' transitions
    std::map<transition, std::set<state>> new_transitions = std::map<transition, std::set<state>>();
    for (state s : fa.getStates()) {

        // Getting current fecho lambda
        std::set<state> fecho_lambda = std::set<state>();
        for (std::pair<state,std::set<state>> p : fechos_lambda) {
            if (p.first == s) {
                fecho_lambda = p.second;
                break;
            }
        }

        // For each symbol in the alphabet
        for (std::string symbol : fa.getAlphabet()) {

            // fλ(δ(s,a))
            std::set<std::set<state>> set_of_fechos = std::set<std::set<state>>();
            for (auto element : fecho_lambda) {
                // δ(s,a)
                for (state s : fa.transite(element,symbol)) {
                    set_of_fechos.insert(fa.getStatesFromLambdaTransition(s));
                }
            }

            // Union of all fλ(δ(s,a))
            std::set<state> set_union = std::set<state>();
            for (std::set<state> fecho : set_of_fechos) {
                set_union = getSetUnion(set_union, fecho);
            }

            // δ'(s,a) = U fλ(δ(s,a))
            new_transitions[std::make_pair(s, symbol)] = set_union;
        }
    }

    // Step 3
    // Updating final states
    std::set<state> new_final_states = fa.getFinalStates();
    std::set<state> initial_state_fecho = fa.getStatesFromLambdaTransition(fa.getInitialState());
    bool has_final_state = false;
    for (state s : initial_state_fecho) {

        if (fa.isFinalState(s)) {
            has_final_state = true;
            break;
        }
    }
    if (has_final_state) {
        new_final_states.insert(fa.getInitialState());
    }

    // Step 4
    // Creating new FA
    return FA(fa.getStates(),
        fa.getAlphabet(),
        new_transitions,
        fa.getInitialState(),
        new_final_states);
}

/**
 * @brief Transforms a Non-Deterministic Finite Automaton into a Deterministic Finite Automaton
 * 
 * @param fa The FA to be transformed
 * @return The transformed FA
 */
FA determinizeFA(FA fa) {
    if (fa.hasLambda()) {
        fa = removeLambdaTransitions(fa);
    }

    SuperFA superFa = SuperFA();

    // Step 1
    // Get transitions and create new required super states
    for (auto transition : fa.getTransitions()) {
        superFa.addState({transition.first.first});
        superFa.addState(transition.second);
        superFa.addTransition({transition.first.first}, transition.first.second, transition.second);
    }

    // Step 2
    // Create new transitions based on new super states  
    for (auto ss : superFa.getStates()) {
        // Case it is a normal transition
        if (ss.size() < 2) {
            continue;
        }
        for (auto symbol : fa.getAlphabet()) {
            superState objSs = superState();
            for (auto s : ss) {
                superState toSs = superFa.transite({s}, symbol);
                objSs = getSetUnion(objSs, toSs);
            }
            // Case the transition does not exist
            if (objSs.size() == 0) {
                continue;
            }
            superFa.addState(objSs);
            superFa.addTransition(ss, symbol, objSs);
        }
    }
    
    // Step 3
    // Update final states and initial state
    for (auto ss : superFa.getStates()) {
        for (auto s : ss) {
            if (fa.isFinalState(s)) {
                superFa.addFinalState(ss);
                break;
            }
        }
    }
    superFa.setInitialState({fa.getInitialState()});

    // Step 4
    // Create new SuperFA
    // Creating alphabet
    for (std::string symbol : fa.getAlphabet()) {
        superFa.addSymbol(symbol);
    }

    // Step 5
    // Convert SuperFA to FA
    FA newFa = superFa.convertToFa();

    newFa.renameAutomatonStates();

    return newFa;
}