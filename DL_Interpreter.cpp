#include <iostream>
#include <string>
#include <vector>
#include <list>
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


Expression* fromEnv(string id)
{
	auto search = env.find(id);
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
	Value(int val) : _val(val){}
	

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
	Variable(string id) : _id(id){}
	

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
	Let(string id, Expression *e1, Expression *e2) : _id(id), _e1(e1), _e2(e2) {}


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
	Function(string id, Expression* exp) : _id(id), _exp(exp) {}


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


list <string> tokens;


void tokenizer(istream& in)
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


Expression* parser()
{
	Expression* res_exp = nullptr;
	string operation;
	while (!tokens.empty()) 
	{
		operation = tokens.front();
		tokens.pop_front();

		if (operation == "(")
		{
			return parser();
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
			res_exp = new Add(parser(), parser());
		}

		else if (operation == "let")
		{
			string id;
			Expression* e1, *e2;
			id = tokens.front();
			tokens.pop_front();

			if (tokens.front() != "=")
				throw "ERROR";

			tokens.pop_front();
			e1 = parser();

			if (tokens.front() != "in")
				throw "ERROR";

			tokens.pop_front();
			e2 = parser();

			res_exp = new Let(id, e1, e2);
		}

		else if (operation == "if")
		{
			Expression* e1, *e2, *e_then, *e_else;
			e1 = parser();
			e2 = parser();

			if (tokens.front() != "then")
				throw "ERROR";

			tokens.pop_front();
			e_then = parser();

			if (tokens.front() != "else")
				throw "ERROR";

			tokens.pop_front();
			e_else = parser();
			res_exp = new If(e1, e2, e_then, e_else);
		}

		else if (operation == "function")
		{
			string id = tokens.front();
			tokens.pop_front();
			res_exp = new Function(id, parser());
		}

		else if (operation == "call")
		{
			Expression* e1, *e2;
			e1 = parser();
			e2 = parser();
			res_exp = new Call(e1, e2);
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
		tokenizer(in);
		Expression *exp = parser();
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