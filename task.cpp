#include "api.hpp"
#include <string>
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <stack>
#include <regex>

struct tree_node {
    char symbol;           // поле данных
    std::set <int> follow_poz;
    std::set <int> last_poz;
    std::set <int> first_poz;
    int poz;
    bool nullable;
    struct tree_node *root;
    struct tree_node *left;  // левый потомок
    struct tree_node *right; // правый потомок
};
int k,sym=0,length;
std::set<int> findpoz(char symbol, std::string str2){
    std::set<int> positions;
    for(int i=0;i<str2.length();i++){
        if(symbol == str2[i]){
            positions.insert(i+1);
        }
    }
    return positions;
}
void up_build_tree(tree_node *t,std::vector<std::set<int>> &table_follow)
{
    if (t == nullptr) return;

    up_build_tree(t->left,table_follow);//С помощью рекурсивного посещаем левое поддерево
    up_build_tree(t->right,table_follow); //С помощью рекурсии посещаем правое поддерево
    auto left = t->left;
    auto right = t->right;
    if(t->symbol == '|'){
        t->nullable = (left->nullable || right->nullable);
        t->first_poz = left->first_poz;
        t->first_poz.insert(right->first_poz.begin(),right->first_poz.end());
        t->last_poz = left->last_poz;
        t->last_poz.insert(right->last_poz.begin(),right->last_poz.end());
    } else if (t->symbol == '&'){
        t->nullable = (left->nullable && right->nullable);
        t->first_poz = left->first_poz;
        if(left->nullable){
            t->first_poz.insert(right->first_poz.begin(),right->first_poz.end());
        }
        t->last_poz = right->last_poz;
        if(right->nullable){
            t->last_poz.insert(left->first_poz.begin(),left->first_poz.end());
        }
        for(int it:left->last_poz){
            table_follow[it-1].insert(right->first_poz.begin(),right->first_poz.end());
        }
    } else if (t->symbol == '*'){
        t->nullable = true;
        t->first_poz = left->first_poz;
        t->last_poz = left->first_poz;
        for(int it:t->last_poz){
            table_follow[it-1].insert(t->first_poz.begin(),t->first_poz.end());
        }
    }
    std::cout << t->symbol << '\t' <<t->poz<< '\t' << t->nullable << '\t';
    for(auto i : t->first_poz)
        std::cout << i << ' ';
    std :: cout << "\t\t\t";
    for(auto i : t->last_poz)
        std::cout << i << ' ';
    std::cout << '\n';
}

std::string replace_str(std::string str,const std::string& find, const std::string& change){
    size_t index = 0;
    size_t pos = str.find(find);
        if (pos != std::string::npos)
            return str;
    while (true) {
        /* Locate the substring to replace. */
        index = str.find(find, index);
        if (index == std::string::npos) break;
        /* Make the replacement. */
        str.replace(index, change.size(), change);
        /* Advance index forward so the next iteration doesn't pick it up as well. */
        index += 3;
    }
    return str;
}
struct tree_node* fill_tree(std::string polis, std::set <char> &symbols){
    bool first = symbols.find(polis[k]) != symbols.end();
    auto leaf = new struct tree_node;
    if (first) {
        leaf->symbol = polis[k];
        leaf->poz = sym;
        if (leaf->symbol != '@') {
            leaf->first_poz.insert(sym);
            leaf->last_poz.insert(sym);
            leaf->nullable = false;
        }
        else {
            leaf->first_poz.insert(0);
            leaf->last_poz.insert(0);
            leaf->nullable = true;
        }
        leaf->left = nullptr;
        leaf->right = nullptr;
        k--;
        sym--;
    }
    else{
        leaf->symbol = polis[k];
        leaf->poz = -1;
        if(polis[k] != '*'){
            k--;
            leaf->right = fill_tree(polis,symbols);
            leaf->left = fill_tree(polis,symbols);
        }else{
            k--;
            leaf->left = fill_tree(polis,symbols);
            leaf->right = nullptr;
        }
    }
    return leaf;
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
    std::vector <int> insert_mul_poz;
    if(*new_string.begin()=='|'){
        new_string.insert(0,"@");
    }
    if(*(new_string.end()-1)=='|'){
        new_string.push_back('@');
    }

    new_string = replace_str(new_string,"(|","(@|");
    new_string = replace_str(new_string,"||","|@|");
    new_string = replace_str(new_string,"|)","|@)");
    std::cout << "insert @ SUCCESS\n";
//    new_string = replace_str(new_string,"|)","|@)");

//    new_string = std::regex_replace(new_string, std::regex("||"), "|@|");
    //    for (auto it = new_string.begin(); it != new_string.end() - 1; ++it){
//        bool first = symbols.find(*it) != symbols.end();
//        bool second = symbols.find(*(it+1)) != symbols.end();
////        std::cout << (!(first || second)) << "\n";
//        if ((it == new_string.begin()) && !first && *it!='('){
//            insert_eps_poz.push_back(-1);
//            count++;
//        }
//        if (!(first || second) && *it != '*' && *(it+1) != '*' &&(*it!=')'&&*(it+1)!='('))  {
//            insert_eps_poz.push_back(i+count);
//            count++;
//        }
//        if((it+1 == new_string.end()-1&&!second) && *(it+1) != '*'){
//            insert_eps_poz.push_back(i+count+1);
//        }
//        i++;
//    }
//    for(int it:insert_eps_poz){
//        new_string.insert(it+1,"@");
////        std::cout<<it<< "\n";
//    }
    i=count=0;

    for (auto it = new_string.begin(); it != new_string.end() - 1; ++it){
        bool first = symbols.find(*it) != symbols.end();
        bool second = symbols.find(*(it+1)) != symbols.end();
        if(first and second){
            insert_mul_poz.push_back(i+count);
            count++;
        }
//        if(!first && !second  && (*it==')' && *(it+1)=='(')){
//            insert_mul_poz.push_back(i+count);
//            count++;
//        }
        if(second and *it=='*'){
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
    new_string.insert(0,"(");
    new_string.push_back(')');
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
    k = polish_notation.length()-1;
    count = 0;
    for(char it:new_string) {
        if (it == '&' || it == '|' || it == '*')
            count++;
    }
    sym = k-count+1;
    length = sym;
    std::vector<std::set<int>>my_sets;
    for(int l = 0; l < sym;l++){
//        std::cout<<l<<'\n';
        std::set<int> sets;
        my_sets.push_back(sets);
    }
    auto* root = fill_tree(polish_notation,symbols);
    up_build_tree(root,my_sets);


    std::cout << '\n' << root->symbol;
    std::cout<<'\n';
    std::cout<<new_string << '\n';
    std::cout<<polish_notation << '\n';
    std::string symb;
    for(char it:polish_notation){
        if(bool(symbols.find(it) != symbols.end())){
            symb.push_back(it);
        }
    }
    for(int l = 0; l < length; l++){
        std::cout << l+1 << '\t';
        for(auto h : my_sets[l])
            std::cout << h << ' ';
        std::cout << '\n';
    }
    std::cout << "ITERSECTION\n";
    std::vector<std::set<int>>condition;
    std::vector<std::set<int>>condition_copy = condition;
    condition.push_back(root->first_poz);
    bool is_in = condition[0].find(symb.size()) != condition[0].end();
    res.create_state("0", is_in);
    res.set_initial("0");
    int elem=0;
    do{
        condition_copy = condition;
        for(char it_2:alph){
            std::set<int>new_condition;
            std::set<int> positions = findpoz(it_2,symb);
            std::set<int> intersect;
            set_intersection(positions.begin(), positions.end(), condition[elem].begin(), condition[elem].end(),
                             std::inserter(intersect, intersect.begin()));
            for(int iter : intersect){
                new_condition.insert(my_sets[iter-1].begin(),my_sets[iter-1].end());
                std::cout << iter << ' ';
            }
            //is it new?
            bool new_cond = true;
            for(int l=0;l != condition.size();l++){
                if(condition[l] == new_condition) {
                    new_cond = false;
                    res.set_trans(std::to_string(elem),it_2,std::to_string(l));
                    break;
                }
            }
            if(new_cond){
                if(!(new_condition.empty())) {

                    condition.push_back(new_condition);
                    is_in = new_condition.find(symb.size()) != new_condition.end();
//                    std::cout << "end " << () << '\n';
                    res.create_state(std::to_string(condition.size()-1), is_in);
                    res.set_trans(std::to_string(elem),it_2,std::to_string(condition.size()-1));

                }
            }

            std::cout<<"NEW\n";
            for(auto iter:condition){
            }
            for(int iter:new_condition){
                std::cout << iter << ' ';
            }
            std::cout<<"\n";
        }

        std::cout << '\n';
        elem++;
    }while(condition_copy.size() != condition.size());
    for(auto it:condition){
        std::cout << "state: ";
        for(int iter:it){
            std::cout<<iter<< ' ';
        }
        std::cout<<'\n';
    }
    std::cout<<symb;
	return res;
}
