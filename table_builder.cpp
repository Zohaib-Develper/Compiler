#include <iostream>
#include <fstream>
#include <cctype>
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
using namespace std;

class CharToIntMap
{
private:
    char mapping[256];
    int size;

public:
    CharToIntMap()
    {
        for (int i = 0; i < 256; i++)
        {
            mapping[i] = -1;
        }
        size = 0;
    }

    bool exists(char value)
    {
        for (int i = 0; i < size; i++)
            if (mapping[i] == value)
                return true;
        return false;
    }

    void insert(char value)
    {
        if (!exists(value))
            mapping[size++] = value;
    }

    int get(char value)
    {
        for (int i = 0; i < size; i++)
            if (mapping[i] == value)
                return i;
        return -1;
    }

    void display()
    {
        cout << "\t";
        for (int i = 0; i < size; i++)
            cout << mapping[i] << " ";
    }

    int length()
    {
        return size;
    }
};

CharToIntMap mapForId;
CharToIntMap mapForPunctuations;
CharToIntMap mapForOperators;

string keywords[] = {
    "asm", "Wagarna", "new", "this", "auto", "enum", "operator", "throw", "Mantiqi", "explicit", "private",
    "True", "break", "export", "protected", "try", "case", "extern", "public", "typedef", "catch", "False", "register", "typeid",
    "Harf", "Ashriya", "typename", "Adadi", "class", "for", "Wapas", "union", "const", "dost", "short", "unsigned", "goto",
    "signed", "using", "continue", "Agar", "sizeof", "virtual", "default", "inline", "static", "Khali", "delete", "volatile", "do",
    "long", "struct", "double", "mutable", "switch", "while", "namespace", "template", "Marqazi", "Matn", "input->", "output<-"};

string punctuations[] = {"[", "{", "(", ")", "}", "]", "::"};

string operators[] = {"=",
                      "<", ">", "<>", ":=", "==", "+", "-", "++", "+=", "<=", ">=", "%", "||", "&&", "!=",
                      "*", "\"", "\"", "/", "<<", ">>"};

const int totalKeywords = 60;
const int totalPunctuations = 7;
const int totalOperators = 22;

// Expected number of states is 255
const int noOfStatesForId = 300, noOfCharsForId = 40;
const int noOfStatesForPunctuations = 50;
const int noOfStatesForOperators = 100, noOfOperators = 20;

int **identifierTable;
int *finalStatesForId;

int **punctuationsTable;
int *finalStatesForPunctuations;

int **operatorsTable;
int *finalStatesForOperators;

const int BUFFER_SIZE = 25; // Each buffer holds 25 characters
char buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];
char *lexemeStart;        // Pointer to track the beginning of a lexeme
char *fwd;                // Pointer for reading the input
bool usingBuffer1 = true; // To switch between buffers
ifstream sourceFile;

void allocateMemory()
{
    // Allocate memory for identifierTable
    identifierTable = new int *[noOfStatesForId];
    for (int i = 0; i < noOfStatesForId; i++)
    {
        identifierTable[i] = new int[noOfCharsForId];
    }
    finalStatesForId = new int[totalKeywords];

    // Allocate memory for punctuationsTable
    punctuationsTable = new int *[noOfStatesForPunctuations];
    for (int i = 0; i < noOfStatesForPunctuations; i++)
    {
        punctuationsTable[i] = new int[totalPunctuations];
    }
    finalStatesForPunctuations = new int[totalPunctuations];

    // Allocate memory for operatorsTable
    operatorsTable = new int *[noOfStatesForOperators];
    for (int i = 0; i < noOfStatesForOperators; i++)
    {
        operatorsTable[i] = new int[noOfOperators];
    }
    finalStatesForOperators = new int[totalOperators];
}

void createMapping()
{
    for (int i = 0; i < totalKeywords; i++)
    {
        for (int j = 0; j < keywords[i].length(); j++)
        {
            mapForId.insert(keywords[i][j]);
        }
    }
    for (int i = 0; i < totalPunctuations; i++)
    {
        for (int j = 0; j < punctuations[i].length(); j++)
        {
            mapForPunctuations.insert(punctuations[i][j]);
        }
    }
    for (int i = 0; i < totalOperators; i++)
    {
        for (int j = 0; j < operators[i].length(); j++)
        {
            mapForOperators.insert(operators[i][j]);
        }
    }
}

void createTable(int **&table, const int &noOfStates, const int &noOfChars, int *&finalStates, const int &length, CharToIntMap &map, string words[])
{
    bool stateInUse[noOfStates]; // Wo go to each state from initial state based on some input,
    // this array stores based on which input, we come to this state from initial state

    for (int i = 0; i < noOfStates; i++)
    {
        for (int j = 0; j < noOfChars; j++)
            table[i][j] = -1;
        stateInUse[i] = false;
    }

    for (int i = 0; i < length; i++)
    {
        finalStates[i] = -1;
    }

    int finalStatesCount = 0;
    for (int i = 0; i < length; i++)
    {
        int state = 0;
        bool matchFound = true;
        for (int j = 0; j < words[i].length(); j++)
        {
            // This loop finds next state, if we have faced the same character before, then we do not need to go to new state
            // Instead we should go to the same state to avoid no determinism
            // And we should do this at initial state only or if we have found match at initial state
            if (table[state][map.get(words[i][j])] != -1 && matchFound)
                state = table[state][map.get(words[i][j])];
            else
            {
                matchFound = false;
                int nextState = -1;
                for (int k = state + 1; k < noOfStates; k++)
                {
                    if (!stateInUse[k])
                    {
                        nextState = k;
                        stateInUse[k] = true;
                        break;
                    }
                }
                table[state][map.get(words[i][j])] = nextState;
                state = nextState;
            }
        }

        finalStates[finalStatesCount++] = state;
        cout << finalStates[finalStatesCount - 1] << "\n";
        // cout << "Keyword: " << words[i] << " FinalState: " << finalStates[finalStatesCount - 1] << endl;
    }
}

// void createTableForIdentifiers()
// {

//     bool stateInUseForId[noOfStatesForId]; // Wo go to each state from initial state based on some input,
//                                            // this array stores based on which input, we come to this state from initial state

//     for (int i = 0; i < noOfStatesForId; i++)
//     {
//         for (int j = 0; j < noOfCharsForId; j++)
//             identifierTable[i][j] = -1;
//         stateInUseForId[i] = false;
//     }

//     for (int i = 0; i < totalKeywords; i++)
//     {
//         finalStatesForId[i] = -1;
//     }

//     int finalStatesCount = 0;
//     for (int i = 0; i < totalKeywords; i++)
//     {
//         int state = 0;
//         bool matchFound = true;
//         for (int j = 0; j < keywords[i].length(); j++)
//         {
//             // This loop finds next state, if we have faced the same character before, then we do not need to go to new state
//             // Instead we should go to the same state to avoid no determinism
//             // And we should do this at initial state only or if we have found match at initial state
//             if (identifierTable[state][mapForId.get(keywords[i][j])] != -1 && matchFound)
//                 state = identifierTable[state][mapForId.get(keywords[i][j])];
//             else
//             {
//                 matchFound = false;
//                 int nextState = -1;
//                 for (int i = state + 1; i < noOfStatesForId; i++)
//                 {
//                     if (!stateInUseForId[i])
//                     {
//                         nextState = i;
//                         stateInUseForId[i] = true;
//                         break;
//                     }
//                 }
//                 identifierTable[state][mapForId.get(keywords[i][j])] = nextState;
//                 state = nextState;
//             }
//         }

//         finalStatesForId[finalStatesCount++] = state;
//         cout << "Keyword: " << keywords[i] << " FinalState: " << finalStatesForId[finalStatesCount - 1] << endl;
//     }
// }

// void createTableForPuntuations()
// {
//     bool stateInUseForPunctuations[totalPunctuations]; // Wo go to each state from initial state based on some input,
//     // this array stores based on which input, we come to this state from initial state
//     for (int i = 0; i < noOfStatesForPunctuations; i++)
//     {
//         for (int j = 0; j < totalPunctuations; j++)
//             punctuationsTable[i][j] = -1;
//         stateInUseForPunctuations[i] = false;
//     }

//     for (int i = 0; i < totalPunctuations; i++)
//     {
//         finalStatesForPunctuations[i] = -1;
//     }

//     int finalStatesCount = 0;
//     for (int i = 0; i < totalPunctuations; i++)
//     {
//         int state = 0;
//         bool matchFound = true;
//         for (int j = 0; j < punctuations[i].length(); j++)
//         {
//             if (punctuationsTable[state][mapForPunctuations.get(punctuations[i][j])] != -1 && matchFound)
//                 state = punctuationsTable[state][mapForId.get(punctuations[i][j])];
//             else
//             {
//                 matchFound = false;
//                 int nextState = -1;
//                 for (int i = state + 1; i < noOfStatesForId; i++)
//                 {
//                     if (!stateInUseForPunctuations[i])
//                     {
//                         nextState = i;
//                         stateInUseForPunctuations[i] = true;
//                         break;
//                     }
//                 }
//                 punctuationsTable[state][mapForPunctuations.get(keywords[i][j])] = nextState;
//                 state = nextState;
//             }
//         }

//         finalStatesForPunctuations[finalStatesCount++] = state;
//         cout << "Keyword: " << operators[i] << " FinalState: " << finalStatesForId[finalStatesCount - 1] << endl;
//     }
// }

void createTableForOperators()
{
    bool stateInUseForOperators[noOfStatesForOperators]; // Track states in use
    for (int i = 0; i < noOfStatesForOperators; i++)
    {
        for (int j = 0; j < noOfOperators; j++)
            operatorsTable[i][j] = -1;     // Initialize operators table
        stateInUseForOperators[i] = false; // Initialize state usage
    }

    for (int i = 0; i < totalOperators; i++)
    {
        finalStatesForOperators[i] = -1; // Initialize final states
    }

    int finalStatesCount = 0;

    for (int i = 0; i < totalOperators; i++)
    {
        int state = 0;
        bool matchFound = true;
        for (int j = 0; j < operators[i].length(); j++)
        {
            // This loop finds the next state. If we’ve faced the same character before,
            // we go to the same state to avoid non-determinism.
            if (operatorsTable[state][mapForOperators.get(operators[i][j])] != -1 && matchFound)
            {
                state = operatorsTable[state][mapForOperators.get(operators[i][j])]; // Transition to the next state
            }
            else
            {
                matchFound = false;
                int nextState = -1;
                for (int k = state + 1; k < noOfStatesForOperators; k++)
                {
                    if (!stateInUseForOperators[k])
                    {
                        nextState = k;
                        stateInUseForOperators[k] = true;
                        break;
                    }
                }
                operatorsTable[state][mapForOperators.get(operators[i][j])] = nextState; // Fill the table
                state = nextState;                                                       // Move to the new state
            }
        }

        // Filling table for 'other' input characters (for operators with special handling)
        if (operators[i] == "<" || operators[i] == ">" || operators[i] == "+" || operators[i] == "=")
        {
            // Marking the last state as final state for 'other' input
            operatorsTable[state][mapForOperators.length()] = noOfStatesForOperators - 1;
        }
        else
        {
            finalStatesForOperators[finalStatesCount++] = state; // Store the final state
        }

        cout << "Operator: " << operators[i] << " Final State: " << finalStatesForOperators[finalStatesCount - 1] << endl;
    }
}

bool loadBuffer()
{
    if (!sourceFile.eof())
    {
        if (usingBuffer1)
        {
            sourceFile.read(buffer1, BUFFER_SIZE);
            fwd = buffer1;
        }
        else
        {
            sourceFile.read(buffer2, BUFFER_SIZE);
            fwd = buffer2;
        }
        usingBuffer1 = !usingBuffer1; // Switch buffers
        return true;
    }
    return false;
}

// Function to get the next character
char getNextChar()
{
    if (*fwd == '\0')
    {
        if (!loadBuffer())
            return EOF;
    }
    return *fwd++;
}

int main()
{
    // Each word in keywords is mapped to some integer
    allocateMemory();
    createMapping();
    // createTable(identifierTable, noOfStatesForId, noOfCharsForId, finalStatesForId, totalKeywords, mapForId, keywords);
    // createTable(punctuationsTable, noOfStatesForPunctuations, totalPunctuations, finalStatesForPunctuations, totalPunctuations, mapForPunctuations, punctuations);
    createTableForOperators();

    mapForOperators.display();
    cout << endl;

    for (int i = 0; i < noOfStatesForOperators; i++)
    {
        cout << i;
        for (int j = 0; j < 26; j++)
        {
            if (i == finalStatesForOperators[j])
                cout << "F";
        }
        cout << " ";
        for (int j = 0; j < noOfOperators; j++)
            cout << operatorsTable[i][j] << " ";
        cout << endl;
    }

    sourceFile.open("source.txt");

    if (!sourceFile.is_open())
    {
        cout << "Error opening file!" << endl;
        return 1;
    }

    loadBuffer(); // Load the first buffer
    // lexicalAnalyzer();
    cout << "Buffer: " << endl;
    while (loadBuffer())
    {
        cout << getNextChar();
    }

    sourceFile.close();
    return 0;
}
