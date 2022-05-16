#include "api.hpp"
#include <string>
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <stack>

struct tree_node {
    char symbol;           // поле данных
    std::set <int> follow_poz;
    std::set <int> last_poz;
    std::set <int> first_poz;
    int poz;
    struct tree_node *root;
    struct tree_node *left;  // левый потомок
    struct tree_node *right; // правый потомок
};

void print_tree(tree_node *t)
{
    if (t == nullptr) return;

    print_tree(t->left);//С помощью рекурсивного посещаем левое поддерево
    print_tree(t->right); //С помощью рекурсии посещаем правое поддерево
    std::cout << t->symbol << '\n'; //И показываем элемент

}


DFA re2dfa(const std::string &s) {
    DFA res = DFA(Alphabet(s));
    std::set<char> symbols;
    auto alph = Alphabet(s);
    for (char it: alph) {
        symbols.insert(it);
        std::cout << it;
    }
    symbols.insert('@');
    symbols.insert('#');
    std::cout << "\n";
    int i = 0,count = 0;
    std::string new_string = s;
    std::vector <int> insert_eps_poz;
    std::vector <int> insert_mul_poz;
    for (auto it = new_string.begin(); it != new_string.end() - 1; ++it){
        bool first = symbols.find(*it) != symbols.end();
        bool second = symbols.find(*(it+1)) != symbols.end();
//        std::cout << (!(first || second)) << "\n";
        if ((it == new_string.begin()) && !first && *it!='('){
            insert_eps_poz.push_back(-1);
            count++;
        }
        if (!(first || second) && *it != '*' && *(it+1) != '*')  {
            insert_eps_poz.push_back(i+count);
            count++;
        }
        if((it+1 == new_string.end()-1&&!second) && *(it+1) != '*'){
            insert_eps_poz.push_back(i+count+1);
        }
        i++;
    }
    for(int it:insert_eps_poz){
        new_string.insert(it+1,"@");
//        std::cout<<it<< "\n";
    }
    i=count=0;

    for (auto it = new_string.begin(); it != new_string.end() - 1; ++it){
        bool first = symbols.find(*it) != symbols.end();
        bool second = symbols.find(*(it+1)) != symbols.end();
        if(first and second){
            insert_mul_poz.push_back(i+count);
            count++;
        }
        i++;
    }
    for(int it:insert_mul_poz){
        new_string.insert(it+1,"&");
        std::cout<<it<< "\n";
    }
    std::cout<<"\n";
    std::vector <char> poz;
    new_string.push_back('&');
    new_string.push_back('#');
    for(char it:new_string){
        if(symbols.find(it) != symbols.end()) {
            poz.push_back(it);
        }
//        std::cout<<it;
    }
    std::string polish_notation;
    std::stack<char> operands;
    // 0 |
    // 1 &
    // 2 *
    // 3 ()
    for(char it:new_string){
        if(!bool(symbols.find(it) != symbols.end())){
            if (it == ')'){
                while(operands.top()!='(') {
                    polish_notation.push_back(operands.top());
                    operands.pop();
                }
                operands.pop();
            }
            else if(!operands.empty()){
                if(it=='('){
                    operands.push(it);
                }
                else if(it == '*'){
                    operands.push(it);
                }
                else if(it == '&'){
                    if (operands.top()=='|'){
                        operands.push(it);
                    }
                    else {
                        while (!operands.empty() && (operands.top() == '*' || operands.top() == '&')) {
                            polish_notation.push_back(operands.top());
                            operands.pop();
                        }
                        operands.push(it);
                    }
                }
                else if(it == '|' ){
                    while (!operands.empty() && (operands.top() == '*' || operands.top() == '&' || operands.top() == '|')){
                        polish_notation.push_back(operands.top());
                        operands.pop();
                    }
                    operands.push(it);
                }
            }
            else{
                operands.push(it);
            }
        }
        else{
            polish_notation.push_back(it);
        }
    }
    while (!operands.empty()){
        polish_notation.push_back(operands.top());
        operands.pop();
    }

    auto* root = new struct tree_node;
    root->symbol = polish_notation[polish_notation.length()-1];
    root->right = nullptr;
    root->left = nullptr;
    root->root = root;
    struct tree_node* tmp_root = root;
    for (int j=polish_notation.length()-2; j >= 0;j--){
        auto* tree = new struct tree_node;
        if(!bool(symbols.find(polish_notation[j]) != symbols.end())){
            if (bool(symbols.find(tmp_root->symbol) != symbols.end())){
                tmp_root = tmp_root->root;
                if(tmp_root->left!= nullptr){
                    std::cout << "WRONG";
                }
                else {
                    if(tmp_root->symbol == '*'){
                        tmp_root->left=tree;
                        tmp_root->right = tree;
                    }
                    else {
                        tmp_root->left = tree;
                    }
                    tree->symbol = polish_notation[j];
                    tree->root = tmp_root;
                    tree->left = nullptr;
                    tree->right = nullptr;
                    tmp_root = tree;
                }
            }
        }
        else{
            if(bool(symbols.find(tmp_root->symbol) != symbols.end())){
                while(bool(symbols.find(tmp_root->symbol) != symbols.end())){
                    tmp_root = tmp_root->root;
                }
            }
//            if (tmp_root->right!= nullptr)
            tmp_root->right = tree;
            tree->symbol = polish_notation[j];
            tree->root = tmp_root;
            tree->left = nullptr;
            tree->right = nullptr;
            tmp_root = tree;
        }
    }
    print_tree(root);
    std::cout << '\n' << root->symbol;
    std::cout<<'\n';
//    std::cout<<new_string << '\n';
    std::cout<<polish_notation;
	res.create_state("Start", true);
	res.set_initial("Start");
	return res;
}
