#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <fstream>
#include <sstream>

using namespace std;

class Expression
{
public:
	virtual Expression* eval() = 0;
	virtual string getString() = 0;
	virtual Expression* copy() = 0;
	~Expression() = default;
};


unordered_map<string, Expression*> env;


Expression* fromEnv(string _id)
{
	auto search = env.find(_id);
	if (search != env.end())
	{
		return search->second;
	}
	else
	{
		return nullptr;
	}
}


class Value : public Expression
{
public:
	Value(int val) : _val(val) {}


	Expression* eval()
	{
		return copy();
	}


	string getString()
	{
		return "(val " + to_string(_val) + ")";
	}


	Expression* copy()
	{
		return new Value(_val);
	}


	int getVal()
	{
		return _val;
	}
private:
	int _val;
};


int getValue(Expression *exp)
{
	Value *res_ptr = dynamic_cast<Value*>(exp->eval());
	if (res_ptr == nullptr)
	{
		throw("ERROR");
	}
	return res_ptr->getVal();
}


class Variable : public Expression
{
public:
	Variable(string _id) : _id(_id) {}


	Expression* eval()
	{
		Expression* var = fromEnv(_id);
		if (var == nullptr)
			throw "ERROR";
		return var;
	}


	string getString()
	{
		return "(var " + _id + ")";
	}


	Expression* copy()
	{
		return new Variable(_id);
	}
private:
	string _id;
	friend class Call;
};


class Add : public Expression
{
public:
	Add(Expression *left, Expression *right) : _left(left), _right(right) {}


	Expression* eval()
	{
		int sum = getValue(_left) + getValue(_right);
		Expression* value = new Value(sum);
		return value;
	}


	Expression* copy()
	{
		return new Add(_left->copy(), _right->copy());
	}


	string getString()
	{
		return "(add " + _left->getString() + " " + _right->getString() + ")";
	}


	~Add()
	{
		delete _left;
		delete _right;
	}
private:
	Expression* _left, *_right;
};


class If : public Expression
{
public:
	If(Expression* e1, Expression* e2, Expression* e_then, Expression* e_else) : _e1(e1), _e2(e2), _e_then(e_then), _e_else(e_else) {}


	Expression* eval()
	{
		if (getValue(_e1->eval()) > getValue(_e2->eval()))
		{
			return _e_then->eval();
		}
		else
		{
			return _e_else->eval();
		}
	}


	string getString()
	{
		return "(if " + _e1->getString() + " " + _e2->getString() + " " + _e_then->getString() + " " + _e_else->getString() + ")";
	}


	Expression* copy()
	{
		return new If(_e1->copy(), _e2->copy(), _e_then->copy(), _e_else->copy());
	}


	~If()
	{
		delete _e1;
		delete _e2;
		delete _e_then;
		delete _e_else;
	}
private:
	Expression *_e1, *_e2, *_e_then, *_e_else;
};


class Let : public Expression
{
public:
	Let(string _id, Expression *e1, Expression *e2) : _id(_id), _e1(e1), _e2(e2) {}


	Expression* eval()
	{
		env[_id] = _e1->eval();
		return _e2->eval();
	}


	string getString()
	{
		return "(let " + _id + " = " + _e1->getString() + " in " + _e2->getString() + ")";
	}


	Expression* copy()
	{
		return new Let(_id, _e1->copy(), _e2->copy());
	}


	~Let()
	{
		delete _e1;
		delete _e2;
	}
private:
	Expression *_e1, *_e2;
	string _id;
};


class Function : public Expression
{
public:
	Function(string _id, Expression* exp) : _id(_id), _exp(exp) {}


	Expression* eval()
	{
		return copy();
	}


	string getString()
	{
		return "(function " + _id + " = " + _exp->getString() + ")";
	}


	Expression* copy()
	{
		return new Function(_id, _exp->copy());
	}
	~Function()
	{
		delete _exp;
	}
private:
	string _id;
	Expression* _exp;
	friend class Call;
};


class Call : public Expression
{
public:
	Call(Expression* f_exp, Expression* arg_exp) : _f_exp(f_exp), _arg_exp(arg_exp) {}


	Expression* eval()
	{
		Function* func = dynamic_cast<Function*>(_f_exp->eval());
		if (func == nullptr)
		{
			throw "err";
		}
		Variable* var = dynamic_cast<Variable*>(_f_exp);
		if (var != nullptr)
		{
			if (fromEnv(var->_id) == nullptr)
			{
				throw "Err";
			}
		}
		env[func->_id] = _arg_exp->eval();
		return func->_exp->eval();
	}

	string getString()
	{
		return "(call " + _f_exp->getString() + " " + _arg_exp->getString() + ")";
	}


	Expression* copy()
	{
		return new Call(_f_exp->copy(), _arg_exp->copy());
	}

	~Call()
	{
		delete _f_exp;
		delete _arg_exp;
	}

private:
	Expression* _f_exp, *_arg_exp;
};


class Set : public Expression
{
public:
	Set(string _id, Expression* e_val) :_id(_id), _e_val(e_val){};

	Expression *eval()
	{
		env[_id] = _e_val->eval();
		return copy();
	}
	Expression* copy()
	{
		return new Set(_id, _e_val->copy());
	}

	string getString()
	{
		return "(set " + _id + " " + _e_val->getString() + ")";
	}
	
	~Set()
	{
		delete _e_val;
	}

private:
	string _id;
	Expression *_e_val;
};

class Block : public Expression
{
public:

	Block(vector<Expression*> &income_vector) : _expr_vector(income_vector) {}

	Expression *eval()
	{
		vector<Expression*> eval_vector;
		for (auto iter : _expr_vector)
		{
			eval_vector.push_back(iter->eval());
		}
		return eval_vector[_expr_vector.size() - 1];

	}

	Expression* copy()
	{
		vector<Expression*> copy_vector;
		for (auto iter : _expr_vector)
		{
			copy_vector.push_back(iter->copy());
		}
		return new Block(copy_vector);
	}

	string getString()
	{
		string result = "";
		result += "(block ";
		for (auto iter : _expr_vector)
		{
			result += iter->getString();
			result+= " ";
		}
		result += ")";
		return result;
	}

	~Block()
	{}

private:
	vector<Expression*> _expr_vector;
};


class Arr : public Expression
{
public:
	Arr(){}

	Arr(vector<Expression*> &income_vector) : _arr(income_vector) {}

	~Arr()
	{}

	Expression *eval()
	{
		vector<Expression*> evals;
		for (auto iter : _arr)
			evals.push_back(iter->eval());
		Expression *arr_eval = new Arr(evals);
		return arr_eval;
	}

	Expression *copy()
	{
		vector<Expression*> copy_vector;
		for (auto iter : _arr)
			copy_vector.push_back(iter->copy());
		return new Arr(copy_vector);
	}

	Expression* operator[](int id)
	{
		if (id < 0 || id > _arr.size() - 1)
		{
			throw "ERROR";
		}
		return _arr[id];
	}

	string getString()
	{
		string result = "";
		result += "(arr ";
		for (size_t i = 0; i < _arr.size(); ++i)
		{
			result += _arr[i]->getString();
			result += " ";
		}
		result += ")";
		return result;
	}

private:
	friend class Gen;
	friend class At;
	vector<Expression*> _arr;
};

class Gen : public Expression
{
public:
	Gen(Expression *e_length, Expression* e_func) :_e_length(e_length), _e_func(e_func) {}

	Expression* eval()
	{
		vector<Expression*> _e_val;
		Arr* gen_arr = new Arr;
		int len = getValue(_e_length->eval());
		for (size_t i = 0; i < len; ++i)
		{
			Expression* arr_el = new Call(_e_func, new Value(i));
			gen_arr->_arr.push_back(arr_el->eval());
		}
		return gen_arr;
	}

	Expression* copy()
	{
		return new Gen(_e_length->copy(), _e_func->copy());
	}

	string getString()
	{
		return "(gen " + _e_length->getString() + " " + _e_func->getString() + ")";
	}

	~Gen()
	{
		delete _e_length;
		delete _e_func;
	}

private:
	Expression* _e_length;
	Expression* _e_func;
};

class At : public Expression
{
public:
	At(Expression* e_array, Expression* e_index) :_e_array(e_array), _e_index(e_index) {}

	Expression *eval()
	{
		Arr* at_arr = dynamic_cast<Arr*>(_e_array->eval());
		if (at_arr == nullptr)
		{
			throw "ERROR";
		}
		int id = getValue(_e_index->eval());
		if (id < 0 || id > at_arr->_arr.size() - 1)
		{
			throw "ERROR";
		}
		return (*at_arr)[id];
	}

	Expression* copy()
	{
		return new At(_e_array, _e_index);
	}

	string getString()
	{
		return "(at " + _e_array->getString() + " " + _e_index->getString() + ")";
	}

	~At()
	{
		delete _e_array;
		delete _e_index;
	}
private:
	Expression* _e_array;
	Expression* _e_index;
};


void tokenizer(istream& in, list <string> &tokens)
{
	string lexem_str;
	stringstream str;

	while (in >> str.rdbuf());

	lexem_str = str.str();

	for (size_t i = 0; i < lexem_str.length(); ++i)
	{
		if (lexem_str[i] == '(')
		{
			lexem_str.insert(i + 1, " ");
		}
		else if (lexem_str[i] == ')')
		{
			lexem_str.insert(i++, " ");
		}
	}
	stringstream lexem_stream(lexem_str);

	while (lexem_stream >> lexem_str)
	{
		tokens.push_back(lexem_str);
	}
}


Expression* parser(list <string> &tokens)
{
	Expression* res_exp = nullptr;
	string operation;
	while (!tokens.empty())
	{
		operation = tokens.front();
		tokens.pop_front();

		if (operation == "(")
		{
			return parser(tokens);
		}

		else if (operation == ")")
		{
			return res_exp;
		}

		else if (operation == "val")
		{
			res_exp = new Value(stoi(tokens.front()));
			tokens.pop_front();
		}

		else if (operation == "var")
		{
			res_exp = new Variable(tokens.front());
			tokens.pop_front();
		}

		else if (operation == "add")
		{
			Expression* e1, *e2;
			e1 = parser(tokens);
			e2 = parser(tokens);
			res_exp = new Add(e1, e2);
		}

		else if (operation == "let")
		{
			string _id;
			Expression* e1, *e2;
			_id = tokens.front();
			tokens.pop_front();

			if (tokens.front() != "=")
				throw "ERROR";

			tokens.pop_front();
			e1 = parser(tokens);

			if (tokens.front() != "in")
				throw "ERROR";

			tokens.pop_front();
			e2 = parser(tokens);

			res_exp = new Let(_id, e1, e2);
		}

		else if (operation == "if")
		{
			Expression* e1, *e2, *e_then, *e_else;
			e1 = parser(tokens);
			e2 = parser(tokens);

			if (tokens.front() != "then")
				throw "ERROR";

			tokens.pop_front();
			e_then = parser(tokens);

			if (tokens.front() != "else")
				throw "ERROR";

			tokens.pop_front();
			e_else = parser(tokens);
			res_exp = new If(e1, e2, e_then, e_else);
		}

		else if (operation == "function")
		{
			string _id = tokens.front();
			tokens.pop_front();
			res_exp = new Function(_id, parser(tokens));
		}

		else if (operation == "call")
		{
			Expression* e1, *e2;
			e1 = parser(tokens);
			e2 = parser(tokens);
			res_exp = new Call(e1, e2);
		}

		else if (operation == "set")
		{
			string _id = tokens.front();
			tokens.pop_front();
			res_exp = new Set(_id, parser(tokens));
		}

		else if (operation == "block")
		{
			vector<Expression*> expr;
			Expression* e1;
			while ((e1 = parser(tokens)) != nullptr)
			{
				expr.push_back(e1);
			}
			res_exp = new Block(expr);
		}

		else if (operation == "arr")
		{
			vector<Expression*> expr;
			Expression* e1;
			while ((e1 = parser(tokens)) != nullptr)
			{
				expr.push_back(e1);
			}
			res_exp = new Arr(expr);
		}

		else if (operation == "gen")
		{
			Expression* e1, *e2;
			e1 = parser(tokens);
			e2 = parser(tokens);
			res_exp = new Gen(e1, e2);
		}

		else if (operation == "at")
		{
			Expression* e1, *e2;
			e1 = parser(tokens);
			e2 = parser(tokens);
			res_exp = new At(e1, e2);
		}

		else
		{
			throw "ERROR";
		}
	}

	throw "ERROR";
}


int main()
{
	ifstream in;
	in.open("input.txt");
	ofstream out;
	out.open("output.txt");
	try
	{
		list <string> tokens;
		tokenizer(in, tokens);
		Expression *exp = parser(tokens);
		Expression *res = exp->eval();
		out << res->getString();
		delete exp;
		delete res;
	}
	catch (...)
	{
		out << "ERROR";
	}
	in.close();
	out.close();
}
