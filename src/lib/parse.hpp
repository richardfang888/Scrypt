# include "lexer.cpp"

struct Node {
    Token token;
    vector<Node*> children;
};


class AST {
    public:
        AST(const vector<Token>& tokens);
        double evaluateAST();
        Node* getRoot() const;
        void printInfix();
        virtual ~AST() {}
    
    private:
        Node *root;
        double evaluate(Node *root);
        Node* buildTree(const vector<Token>& tokens, int index, int eindex);

};
