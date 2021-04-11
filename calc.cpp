#include <iostream>
#include <cmath>
#include <string>
#include <algorithm>
#include <stack>
#include <sstream>
#include <vector>

using namespace std;
double EPS = 0.00001;
enum class ERR {
    DIV_BY_0,
    WRONG_SYMBOL,
    INCORRECT_EXPR
};

/**
 * @brief 
 * 
 * @param c - проверяемый символ
 * @return true - символ является обозначением поддерживаемой операции
 * @return false - иначе
 */
bool isOper (const char c) {
    return c == '+' || c == '-' || c == '*' || c == '/';
}

/**
 * @brief 
 * 
 * @param c - символ операции
 * @return double приоритет операции, большему значению соответствует больший приоритет
 *                -1 в случае неподдерживаемой операции
 */
double priority (const char c) {
    switch (c) {
        case '+':
        case '-':
            return 1;    
            break;
        case '*':
        case '/':
            return 2;    
            break;
        case '(':
            return 0;
            break;
        default:
            return -1;
            break;
    }
}
/**
 * @brief 
 * 
 * @param expr - строка с математическим выражением
 * @return vector<string> - вектор строк, содержащий ОПН исходного выражения
 */
vector<string> createRPN (string expr) {
    string::size_type i;
    do {                        //wipe out spaces
        i = expr.find(' ');                         
        if (i != string::npos)
            expr.erase(i, 1);
    } while (i != string::npos);

    for (int i = 0; i < expr.size(); i++) {      //replace unary minus for expression (0-x)
        if ((expr[i] == '-') && (i == 0 || (!isdigit(expr[i - 1]) && 
            expr[i - 1] != ')' ))) {
            int j = 0;
            for (j = i + 1; j < expr.size(); j++) {
                if (!isdigit(expr[j]) && (expr[j] != '.'))
                    break;
            }
            expr.insert(j, ")");
            expr.insert(i, "(0");
        }
    }

    for (int i = 0; i < expr.size(); i++) {     //add '*' between )( 
        if ((expr[i] == ')') && (expr[i + 1] == '(')) {
            expr.insert(i + 1, "*");
        }
    }

    vector<string> rpn;
    stack<char> oper;
    for (i = 0; i < expr.size(); i++) {
        char c = expr[i];
        if (!isdigit(c) && (c != '.')) {
            if (c == '(') {
                oper.push(c);
            }
            else if (c == ')') {
                while (oper.top() != '(') {
                    string tmp;
                    tmp.push_back(oper.top());
                    rpn.push_back(tmp);
                    oper.pop();
                }
                oper.pop();
            }
            else if (oper.empty() || (priority(oper.top()) < priority(c))) {
                oper.push(c);
            }
            else {
                do {
                    string tmp;
                    tmp.push_back(oper.top());
                    rpn.push_back(tmp);
                    oper.pop();
                } while (!(oper.empty() || (priority(oper.top()) < priority(c))));
                oper.push(c);
            }
        }
        else {
            double num = 0;
            int exp = 0;
            bool flag = 0;
            while (isdigit(expr[i]) || expr[i] == '.') {
                if (expr[i] == '.') {
                    flag = 1;
                }
                else if (isdigit(expr[i])) {
                    num *= 10;
                    num += expr[i] - '0';
                    if (flag)
                        exp--;
                }
                i++;
            }
            num *= pow(10, exp);
            rpn.push_back(to_string(num));
            i--;
        }
    }

    while (!oper.empty()) {
        string tmp;
        tmp.push_back(oper.top());
        rpn.push_back(tmp);
        oper.pop();
    }

    return rpn;
}
/**
 * @brief 
 * 
 * @param rpn - вектор строк, содержащий ОПН исходного выражения
 * @return double - вычисленное значение
 * @throw ERR::DIV_BY_0 в случае попытки деления на 0
 */
double calcRPN (vector<string> rpn) {
    while (rpn.size() != 1) {
        for (int i = 0; i < rpn.size(); i++) {
            if (isOper(rpn[i][0]) && (rpn[i].length() == 1)) {
                switch (rpn[i][0]) {
                    case '+':
                        rpn[i - 2] = to_string(stod(rpn[i - 2]) + stod(rpn[i - 1]));
                        break;
                    case '-':
                        rpn[i - 2] = to_string(stod(rpn[i - 2]) - stod(rpn[i - 1]));
                        break;
                    case '*':
                        rpn[i - 2] = to_string(stod(rpn[i - 2]) * stod(rpn[i - 1]));
                        break;
                    case '/':
                        if (abs(stod(rpn[i - 1])) < EPS) {
                            throw ERR::DIV_BY_0;
                        }
                        rpn[i - 2] = to_string(stod(rpn[i - 2]) / stod(rpn[i - 1]));
                        break;
                    default:
                        break;
                }
                rpn.erase(rpn.begin() + i - 1, rpn.begin() + i + 1);
                i = 0;
            }
        }
    }
    return stod(rpn[0]);
}
/**
 * @brief 
 * 
 * @param expr - строка с математическим выражением
 * @return double - вычисленное значение
 * @throw  ERR::WRONG_SYMBOL в случае содержания исходным выражением символа, отличного от знаков
 *              поддерживаемых операций, скобок, точки или цифр
 *         ERR::INCORRECT_EXPR в случае отсутствия цифр в исходном выражении
 */
double calcExpr (string expr) {
    bool flagExprContainDigit = 0;
    for (const auto &c : expr) {
        if (!isdigit(c) && c != '.' && c != '(' && c != ')' && !isOper(c) && c != ' '){
            throw ERR::WRONG_SYMBOL;
        }
        if (isdigit(c)) {
            flagExprContainDigit = 1;
        }
    }

    if (!flagExprContainDigit) {
        throw ERR::INCORRECT_EXPR;
    }

    vector<string> rpn = createRPN(expr);

    return calcRPN(rpn);
}

void tests () {
    int cSuccess = 0;
    int cTests = 0;

    try {                           //1: simple imput
        cTests++;
        if (calcExpr("0") == 0) {
            cSuccess++;
        } else {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        }
    }
    catch(const ERR x) {
        cout << "Test " << cTests << " failed with error\n";
    }

    try {                           //2: priority
        cTests++;
        if ((calcExpr("2.0+2.0*2") == 6) && (calcExpr("(2+2)/2.0") == 2)) {
            cSuccess++;
        } else {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        }
    }
    catch(const ERR x) {
        cout << "Test " << cTests << " failed with error\n";
    }

    try {                           //3: empty imput
        cTests++;
        if (calcExpr("") == 0) {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        } else {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        }
    }
    catch(const ERR x) {
        if (x == ERR::INCORRECT_EXPR) {
            cSuccess++;
        } else {
            cout << "Test " << cTests << " failed with error\n";
        }
    }

    try {                           //4: using wrong symbols
        cTests++;
        if (calcExpr("i++") == 0) {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        } else {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        }
    }
    catch(const ERR x) {
        if (x == ERR::WRONG_SYMBOL) {
            cSuccess++;
        } else {
            cout << "Test " << cTests << " failed with error\n";
        }
    }

    try {                           //5: first number is negative
        cTests++;
        if (calcExpr("-2+2") == 0) {
            cSuccess++;
        } else {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        }
    }
    catch(const ERR x) {
        cout << "Test " << cTests << " failed with error\n";
    }

    try {                           //6: adding * between )(
        cTests++;
        if (calcExpr("(6)(6)") == 36) {
            cSuccess++;
        } else {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        }
    }
    catch(const ERR x) {
        cout << "Test " << cTests << " failed with error\n";
    } 

    try {                           //7: multiple spaces
        cTests++;
        if (calcExpr("2 +   2*  2") == 6) {
            cSuccess++;
        } else {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        }
    }
    catch(const ERR x) {
        cout << "Test " << cTests << " failed with error\n";
    }

    try {                           //8: division by 0
        cTests++;
        if (calcExpr("1/0") == 0) {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        } else {
            cout << "Test " << cTests <<" failed with incorrect answer\n";
        }
    }
    catch(const ERR x) {
        if (x == ERR::DIV_BY_0) {
            cSuccess++;
        } else {
            cout << "Test " << cTests << " failed with error\n";
        }
    }

    cout << "Successful tests: " << cSuccess << " out of " << cTests << endl;
}

int main(int argc, char const *argv[]) {
    tests();
    return 0;
}
