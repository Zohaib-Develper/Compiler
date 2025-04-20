#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class String2DArray
{
private:
    vector<string> rowKeys;
    vector<string> colKeys;
    vector<string> finalStates;
    vector<vector<int>> data;

    // Finds the index of a key in a given vector, adds it if not found
    int getOrAddIndex(string key, vector<string> &keys)
    {

        if (key.length() > 0 && key[key.length() - 1] == 'F' && key[0] != 'F')
        {
            // Two F final state means move pointer backward
            key[key.length() - 1] = '\0';
            if (find(finalStates.begin(), finalStates.end(), key) == finalStates.end())
            {
                // cout << key << "AND: " << key.length() - 1 << endl;
                ;
                finalStates.push_back(key);
            }
            if (key[key.length() - 2] == 'F')
            {
                key[key.length() - 2] = '\0';
            }
        }
        for (size_t i = 0; i < keys.size(); i++)
        {
            if (keys[i] == key)
                return i;
        }
        keys.push_back(key);
        return keys.size() - 1;
    }

    int getIndex(const string &key, vector<string> &keys)
    {
        for (size_t i = 0; i < keys.size(); i++)
        {
            keys[i].erase(remove(keys[i].begin(), keys[i].end(), '\0'), keys[i].end());
            if (keys[i].compare(key) == 0)
            {
                return i;
            }
        }
        return -1;
    }

    // Resizes the data matrix dynamically while preserving old values
    void resizeMatrix()
    {
        int newRowSize = rowKeys.size();
        int newColSize = colKeys.size();

        vector<vector<int>> newData(newRowSize, vector<int>(newColSize, 0));

        // Copy old data into new resized matrix
        for (size_t i = 0; i < data.size(); i++)
        {
            for (size_t j = 0; j < data[i].size(); j++)
            {
                newData[i][j] = data[i][j];
            }
        }

        data = newData;
    }

public:
    String2DArray() {} // Default constructor

    void setValue(const string &rowKey, string colKey, int value)
    {

        if (rowKey[0] == 'F')
        {
            cout << "YES" << endl;
            cout << rowKey << endl;
        }
        int rowIndex = getOrAddIndex(rowKey, rowKeys);
        int colIndex = getOrAddIndex(colKey, colKeys);

        resizeMatrix(); // Adjust matrix size dynamically

        data[rowIndex][colIndex] = value;
    }

    int getValue(const string &rowKey, string colKey)
    {

        int rowIndex = getIndex(rowKey, rowKeys);

        int colIndex = getIndex(colKey, colKeys);
        string OriginalColKey = colKey;

        if (colIndex == -1)
        {

            if ((colKey[0] >= 65 && colKey[0] <= 90) || (colKey[0] >= 97 && colKey[0] <= 122))
            {
                colKey = "letter";
            }
            else if (colKey[0] >= 48 && colKey[0] <= 57)
                colKey = "digit";
            else if (colKey[0] == '_')
                colKey = "underscore";
            else if (colKey[0] == '+' || colKey[0] == '-')
                colKey = "sign";
            else if (colKey[0] == '.')
                colKey = "dot";

            colIndex = getIndex(colKey, colKeys);
            // Special case since e or E are also letters
            if (colIndex == -1 && (OriginalColKey[0] == 'e' || OriginalColKey[0] == 'E'))
            {
                colIndex = getIndex("exponent", colKeys);
            }
            if (colIndex == -1)
            {

                colIndex = getIndex("other", colKeys);

                if (colIndex == -1)
                    return colIndex;
            }
        }

        resizeMatrix(); // Ensure correct size before accessing

        return data[rowIndex][colIndex];
    }

    void printArray()
    {
        cout << "   ";
        for (const auto &colKey : colKeys)
        {
            cout << colKey << "\t";
        }
        cout << endl;

        for (size_t i = 0; i < rowKeys.size(); i++)
        {
            cout << rowKeys[i] << " ";
            for (size_t j = 0; j < colKeys.size(); j++)
            {
                cout << data[i][j] << "\t";
            }
            cout << endl;
        }
    }
    bool isFinal(int state, bool &movePointerBackward)
    {
        for (int i = 0; i < finalStates.size(); i++)
        {

            if (state == stoi(finalStates[i]))
            {
                if (finalStates[i][finalStates[i].length() - 1] == 'F')
                    movePointerBackward = true;
                return true;
            }
        }
        return false;
    }
};

vector<string> split(const string &str, char delimiter)
{
    vector<string> result;
    size_t start = 0, end;

    while ((end = str.find(delimiter, start)) != string::npos)
    {
        result.push_back(str.substr(start, end - start));
        start = end + 1;
    }
    result.push_back(str.substr(start)); // Last token

    return result;
}

void readCSV(const string &filename, String2DArray &nestedMap)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error opening file!" << endl;
        return;
    }

    string line;
    vector<string> headers;

    // Read first row (headers)
    if (getline(file, line))
    {
        line.erase(0, 1);
        headers = split(line, ',');
    }

    while (getline(file, line))
    {
        stringstream ss(line);
        string rowKey, value;
        getline(ss, rowKey, ',');

        int colIndex = 0;
        while (getline(ss, value, ','))
        {
            if (colIndex < headers.size())
            {
                if (rowKey[0] == 'F')
                {
                    cout << "YES" << endl;
                    cout << rowKey << endl;
                }
                nestedMap.setValue(rowKey, headers[colIndex], stoi(value));
            }
            colIndex++;
        }
    }

    file.close();
}

// Global Vars
String2DArray punctuations;
String2DArray keywords;
String2DArray numbers;
String2DArray identifiers;
String2DArray operators;

class DoubleBufferReader
{
private:
    ifstream file;
    char buffer1[25];
    char buffer2[25];
    char *currentBuffer;
    size_t currentPos;
    size_t currentSize;
    vector<char> readHistory;
    size_t unreadCount;
    int lineCount;

public:
    DoubleBufferReader(const string &filename)
        : currentPos(0), currentSize(0), currentBuffer(nullptr), unreadCount(0)
    {
        lineCount = 0;
        file.open(filename, ios::binary);
        if (!file.is_open())
        {
            throw runtime_error("Failed to open file: " + filename);
        }

        fillBuffer(buffer1);
        currentBuffer = buffer1;
    }

    ~DoubleBufferReader()
    {
        if (file.is_open())
        {
            file.close();
        }
    }

    void fillBuffer(char *buffer)
    {
        file.read(buffer, 25);
        currentSize = file.gcount();
    }

    char readChar()
    {
        if (unreadCount > 0)
        {
            char c = readHistory[readHistory.size() - unreadCount];
            unreadCount--;
            return c;
        }

        if (currentPos >= currentSize)
        {
            if (currentBuffer == buffer1)
            {
                fillBuffer(buffer2);
                currentBuffer = buffer2;
            }
            else
            {
                fillBuffer(buffer1);
                currentBuffer = buffer1;
            }
            currentPos = 0;

            if (currentSize == 0)
            {
                return EOF;
            }
        }

        char c = currentBuffer[currentPos++];
        readHistory.push_back(c);
        return c;
    }

    void unreadChar()
    {
        if (readHistory.empty() || unreadCount >= readHistory.size())
        {
            return;
        }
        unreadCount++;
    }

    bool eof() const
    {
        return unreadCount == 0 && currentPos >= currentSize && file.eof();
    }

    size_t unreadableCount() const
    {
        return readHistory.size() - unreadCount;
    }
    void updateLineCount()
    {
        lineCount++;
    }
    int GetLineCount()
    {
        return lineCount;
    }
};

DoubleBufferReader buffer("source.txt");
struct Token
{
    string type;
    string lexeme;
};

string identifyLexem(String2DArray &transitionTable)
{

    bool movePointerBackward = false;
    string lexem = "";
    char c;
    int nextState = 0;
    int i = 0;

    int readCount = 0;
    while (!buffer.eof())
    {
        // if (bufferLength == 0 || forwardPointer >= bufferLength)
        //     loadBuffer();
        // if (useLastChar)
        // {
        //     c = lastChar;
        //     useLastChar = false;
        // }
        // else
        c = buffer.readChar();
        readCount++;
        // if (c == EOF)
        // {
        //     inputEnded = true;
        //     return lexem;
        // }

        // Litteral
        if (c == '\"')
        {
            return "\"";
        }
        if (c == ' ' || c == '\n')

        {
            if (c == '\n')
                buffer.updateLineCount();
            // for (int i = 0; i < readCount - 1; i++)
            //     buffer.unreadChar();
            // // forwardPointer++;
            return lexem;
        }

        nextState = transitionTable.getValue(to_string(nextState), string() + c);
        // cout << "NEXT STATE FOR :" << c << " is" << nextState << endl;

        if (nextState == -1 || nextState == 0)
        {
            cout << "UNREADING" << endl;

            for (int i = 0; i < readCount; i++)
                buffer.unreadChar();
            return "";
            // return lexem;
        }

        lexem += c;
        // forwardPointer++;
        //  cout << "LEXEM: " << lexem << "And nextState: " << nextState << ": " << transitionTable.isFinal(nextState, movePointerBackward) << endl;

        if (transitionTable.isFinal(nextState, movePointerBackward))
        {
            c = buffer.readChar();
            int tempnextState = transitionTable.getValue(to_string(nextState), string() + c);
            if (!buffer.eof())
                buffer.unreadChar();
            else
                return lexem;
            // Read further input if there is still valid next state
            if (tempnextState != -1 && tempnextState != 0)
                continue;

            if (movePointerBackward)
                buffer.unreadChar();
            cout << "TEMP: " << c << endl;
            cout << "RETURNING" << endl;

            return lexem;
        }
    }
    return "";
}

vector<Token> Compile(string filename)
{
    // Open token output file.
    ofstream tokenFile("token.txt");
    ofstream errorFile("error.txt");
    vector<Token> tokens;

    int i = 0;
    // Process tokens until EOF is reached.

    try
    {
        while (true)
        {
            i++;
            // cout << "BUFFER: " << buffer << endl;

            Token token;
            token.lexeme = "";

            token.lexeme = identifyLexem(punctuations);
            cout << "RETURNED: " << token.lexeme << endl;

            // Literal case
            if (token.lexeme == "\"")
            {
                token.type = "Literal";
                token.lexeme = "";
                char c = '\0';
                while (true)
                {
                    c = buffer.readChar();
                    if (c == '\"')
                        break;
                    token.lexeme += c;
                }
            }
            else if (token.lexeme.length() > 0)
                token.type = "punctuation";
            else
            {
                cout << "Checking for keyword" << endl;
                token.lexeme = identifyLexem(keywords);
                if (token.lexeme.length() > 0)
                    token.type = "keyword";
                else
                {
                    token.lexeme = identifyLexem(numbers);
                    if (token.lexeme.length() > 0)
                        token.type = "number";
                    else
                    {
                        token.lexeme = identifyLexem(identifiers);
                        if (token.lexeme.length() > 0)
                        {
                            cout << "IDENTIFIER " << endl;
                            token.type = "identifier";
                        }
                        else
                        {
                            token.lexeme = identifyLexem(operators);
                            if (token.lexeme.length() > 0)
                                token.type = "operators";
                            cout << "I: " << i << endl;
                            if (i == 2)
                            {
                                throw buffer.GetLineCount();
                                i = 0;
                            }
                        }
                    }
                }
            }

            // cout << "Lexem: " << token.lexeme << endl;

            // Output token details (you can format this as needed).
            if (token.lexeme.length() > 0)
            {
                i = 0;
                tokens.push_back(token);
                cout << "Token Type: " << token.type << "  Lexeme: " << token.lexeme << "\n";
                tokenFile << "Token Type: " << token.type << "  Lexeme: " << token.lexeme << "\n";
            }
            if (buffer.eof())
                break;
        }
    }
    catch (...)
    {
        errorFile << "Error at line: " << buffer.GetLineCount() + 1;
        errorFile.close();
    }
    tokenFile.close();
    return tokens;
}

struct ParseNode
{
    string label;                 // Non-terminal or token
    vector<ParseNode *> children; // Child nodes

    ParseNode(const string &l) : label(l) {}

    void addChild(ParseNode *child)
    {
        if (child)
            children.push_back(child);
    }

    void print(int indent = 0) const
    {
        cout << string(indent, ' ') << label << endl;
        for (const auto &child : children)
        {
            child->print(indent + 2);
        }
    }
};

class Parser
{
private:
    vector<Token> tokens;
    int current;

    Token peek()
    {
        if (current < tokens.size())
            return tokens[current];
        return {"", ""};
    }

    Token get()
    {
        if (current < tokens.size())
            return tokens[current++];
        return {"", ""};
    }

    void error(const string &msg)
    {
        cout << "Error: " << msg << " at token: " << peek().lexeme << endl;
        exit(1);
    }

    bool isTypeToken(const Token &token)
    {
        return token.lexeme == "Adadi" || token.lexeme == "Ashriya" || token.lexeme == "Harf" || token.lexeme == "Matn" || token.lexeme == "Mantiqi";
    }

    bool isCompare(const Token &token)
    {
        return token.lexeme == "==" || token.lexeme == "<" || token.lexeme == ">" ||
               token.lexeme == "<=" || token.lexeme == ">=" || token.lexeme == "!=" ||
               token.lexeme == "<>";
    }

    ParseNode *match(string expectedLexeme)
    {
        Token t = get();
        if (t.lexeme != expectedLexeme)
            error("Expected '" + expectedLexeme + "'");
        return new ParseNode("'" + t.lexeme + "'");
    }

    ParseNode *matchTypeProduction()
    {
        Token t = get();
        if (!isTypeToken(t))
            error("Expected 'Type Keyword'");
        ParseNode *typeNode = new ParseNode("Type");
        typeNode->addChild(new ParseNode("'" + t.lexeme + "'"));
        return typeNode;
    }

    ParseNode *matchType(string expectedType)
    {
        Token t = get();
        if (t.type != expectedType)
            error("Expected type " + expectedType);
        return new ParseNode(t.lexeme);
    }

    ParseNode *Function()
    {
        ParseNode *node = new ParseNode("Function");
        node->addChild(matchTypeProduction()); // type
        node->addChild(matchType("identifier"));
        node->addChild(match("("));

        node->addChild(ArgList());
        cout << "GOING FOR SECONGD:" << peek().lexeme << endl;
        node->addChild(match(")"));

        node->addChild(CompStmt());
        return node;
    }

    ParseNode *ArgPrime()
    {
        ParseNode *node = new ParseNode("ArgPrime");
        while (peek().lexeme == ",")
        {
            node->addChild(match(","));
            node->addChild(Arg());
        }
        return node;
    }

    ParseNode *ArgList()
    {
        ParseNode *node = new ParseNode("ArgList");
        if (isTypeToken(peek()))
            node->addChild(Arg());
        node->addChild(ArgPrime());
        return node;
    }

    ParseNode *Arg()
    {
        ParseNode *node = new ParseNode("Arg");
        node->addChild(matchTypeProduction()); // type
        node->addChild(matchType("identifier"));
        return node;
    }

    ParseNode *Declaration()
    {
        ParseNode *node = new ParseNode("Declaration");
        node->addChild(match(peek().lexeme));
        node->addChild(IdentList());
        node->addChild(match("::"));
        return node;
    }
    ParseNode *BPrime()
    {
        ParseNode *node = new ParseNode("BPrime");
        if (peek().lexeme == ",")
        {
            node->addChild(match(","));
            node->addChild(IdentList());
            node->addChild(BPrime());
        }
        return node;
    }

    ParseNode *IdentList()
    {
        ParseNode *node = new ParseNode("IdentList");
        node->addChild(matchType("identifier"));
        node->addChild(BPrime());
        return node;
    }

    ParseNode *Stmt()
    {
        ParseNode *node = new ParseNode("Stmt");
        cout << "STMT: " << peek().lexeme << endl;
        Token t = peek();
        if (t.lexeme == "for")
            node->addChild(ForStmt());
        else if (t.lexeme == "while")
            node->addChild(WhileStmt());
        else if (t.lexeme == "Agar")
            node->addChild(IfStmt());
        else if (t.lexeme == "{")
            node->addChild(CompStmt());
        else if (isTypeToken(t))
            node->addChild(Declaration());
        else if (t.type == "identifier")
        {
            node->addChild(Expr());
            node->addChild(match("::"));
        }
        else if (t.lexeme == "::")
        {
            node->addChild(match("::"));
        }
        else
        {
            error("Invalid statement");
        }

        cout << "GOING OUT" << peek().lexeme << endl;
        return node;
    }

    ParseNode *ForStmt()
    {
        ParseNode *node = new ParseNode("ForStmt");
        node->addChild(match("for"));
        node->addChild(match("("));
        node->addChild(Expr());
        node->addChild(match("::"));
        if (peek().lexeme != "::")
            node->addChild(Expr());
        node->addChild(match("::"));
        if (peek().lexeme != ")")
            node->addChild(Expr());
        node->addChild(match(")"));
        node->addChild(Stmt());
        return node;
    }

    ParseNode *WhileStmt()
    {
        ParseNode *node = new ParseNode("WhileStmt");
        node->addChild(match("while"));
        node->addChild(match("("));
        node->addChild(Expr());
        node->addChild(match(")"));
        node->addChild(Stmt());
        return node;
    }

    ParseNode *IfStmt()
    {
        cout << "IF STMT" << endl;
        ParseNode *node = new ParseNode("IfStmt");
        cout << "CHECKING FOR STMT: " << peek().lexeme << endl;
        node->addChild(match("Agar"));
        node->addChild(match("("));
        node->addChild(Expr());
        node->addChild(match(")"));
        node->addChild(Stmt());
        node->addChild(ElsePart());
        return node;
    }

    ParseNode *ElsePart()
    {
        ParseNode *node = new ParseNode("ElsePart");
        if (peek().lexeme == "Wagarna")
        {
            node->addChild(match("Wagarna"));
            node->addChild(Stmt());
        }
        return node;
    }

    ParseNode *CompStmt()
    {
        ParseNode *node = new ParseNode("CompStmt");
        node->addChild(match("{"));
        node->addChild(StmtList());
        cout << "CUR: " << peek().lexeme << endl;
        node->addChild(match("}"));
        return node;
    }

    ParseNode *CPrime()
    {
        ParseNode *node = new ParseNode("CPrime");
        // while (peek().lexeme != "}" && peek().lexeme != "")
        //{
        node->addChild(Stmt());
        cout << "PR: " << peek().lexeme << endl;
        if (peek().lexeme != "}" && peek().lexeme != "")
            node->addChild(CPrime());
        return node;
        //}
    }
    ParseNode *StmtList()
    {
        ParseNode *node = new ParseNode("StmtList");
        node->addChild(CPrime());
        cout << "@: " << peek().lexeme << endl;
        return node;
    }

    ParseNode *Expr()
    {
        ParseNode *node = new ParseNode("Expr");
        Token t = peek();
        Token next = tokens[current + 1];

        if (next.lexeme == ":=")
        {
            node->addChild(matchType("identifier"));
            node->addChild(match(":="));
            node->addChild(Expr());
            return node;
        }
        else
        {
            cout << "RVALUE: " << endl;
            cout << "PEEK " << peek().lexeme << endl;
            node->addChild(Rvalue());
        }
        return node;
    }

    ParseNode *DPrime()
    {
        ParseNode *node = new ParseNode("DPrime");
        if (isCompare(peek()))
        {
            node->addChild(match(peek().lexeme));
            node->addChild(Mag());
            node->addChild(DPrime());
        }
        return node;
    }
    ParseNode *Rvalue()
    {
        ParseNode *node = new ParseNode("Rvalue");
        node->addChild(Mag());
        node->addChild(DPrime());
        return node;
    }
    ParseNode *EPrime()
    {
        ParseNode *node = new ParseNode("EPrime");
        if (peek().lexeme == "+" || peek().lexeme == "-")
        {
            node->addChild(match(peek().lexeme));
            node->addChild(Term());
            node->addChild(EPrime());
        }
        return node;
    }

    ParseNode *Mag()
    {
        ParseNode *node = new ParseNode("Mag");
        node->addChild(Term());
        node->addChild(EPrime());
        return node;
    }
    ParseNode *HPrime()
    {
        ParseNode *node = new ParseNode("HPrime");
        if (peek().lexeme == "*" || peek().lexeme == "/")
        {
            node->addChild(match(peek().lexeme));
            node->addChild(Factor());
            node->addChild(HPrime());
        }
        return node;
    }
    ParseNode *Term()
    {
        ParseNode *node = new ParseNode("Term");
        node->addChild(Factor());
        node->addChild(HPrime());

        return node;
    }

    ParseNode *Factor()
    {
        ParseNode *node = new ParseNode("Factor");
        Token t = peek();
        if (t.lexeme == "(")
        {
            node->addChild(match("("));
            node->addChild(Expr());
            node->addChild(match(")"));
        }
        else if (t.type == "identifier" || t.type == "number")
        {
            node->addChild(matchType(t.type));
        }
        else
        {
            error("Expected factor");
        }
        return node;
    }

public:
    Parser(const vector<Token> &tokenList) : tokens(tokenList), current(0) {}

    void parse()
    {
        ParseNode *root = Function();
        if (current != tokens.size())
        {
            error("Unexpected token at end");
        }
        cout << "Parse Tree:\n";
        root->print();
    }
};
int main()
{
    readCSV("punctuations.csv", punctuations);
    readCSV("keywords.csv", keywords);
    readCSV("numbers.csv", numbers);
    readCSV("identifiers.csv", identifiers);
    readCSV("operators.csv", operators);
    vector<Token> tokens = Compile("source.txt");
    cout << "\n\nParsing: " << endl;
    if (tokens[0].lexeme == "Adadi")
        cout << "Token 0: " << tokens[0].lexeme << endl;
    Parser parser(tokens);
    parser.parse();

    return 0;
}