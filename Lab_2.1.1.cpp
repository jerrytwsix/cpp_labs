#include <iostream>
#include <string>
#include <stack>


class Equation {
public:
	virtual Equation* derivative(std::string var) = 0;
	virtual int eval(std::string term) = 0;
	virtual std::string eq_to_str() = 0;
	virtual void print() = 0;
	virtual Equation* simplify_equation() = 0;			
	virtual bool is_variable() = 0;
	virtual ~Equation() = default;
};


class Number : public Equation {
private:
	uint32_t number;
public:
	Number() : number(0) {}

	
	Number(uint32_t number) : number(number) {}


	Equation* derivative(std::string var)
	{
		return new Number(0);
	}

	
	int eval(std::string term)
	{
		return number;
	}

	
	std::string eq_to_str()
	{
		return std::to_string(number);
	}

	
	void print()
	{
		std::cout << this->eq_to_str();
	}


	Equation* simplify_equation() 
	{
		return new Number(number);
	}

	
	bool is_variable() 
	{
		return false;
	}
};


class Variable : public Equation 
{
private:
	std::string var;
public:
	Variable() : var("x") {}

	
	Variable(std::string var) : var(var) {}

	
	Equation* derivative(std::string var)
	{
		if (this->var == var)
			return new Number(1);
		else
			return new Number(0);
	}


	int eval(std::string term)
	{
		for (size_t i = 0; i < term.length(); ++i)
		{
			std::string check_var = "";

			while (term[i] != ' ')
			{
				check_var += term[i++];
			}

			if (check_var == var)
			{
				i += 4;
				int value = 0;

				while (term[i] != ';' && i < term.length())
					value = value * 10 + (static_cast<int>(term[i++]) - static_cast<int>('0'));
				return value;
			}
			else
			{
				for (; term[i] != ';' && i < term.length(); i++);
				i++;
			}
		}
	}


	std::string eq_to_str()
	{
		return var;
	}

	
	void print()
	{
		std::cout << this->eq_to_str();
	}

	
	Equation* simplify_equation() 
	{
		return new Variable(var);
	}

	
	bool is_variable() 
	{
		return true;
	}
};


class Add : public Equation 
{
private:
	Equation* left, *right;

public:
	Add() : left(nullptr), right(nullptr) {}

	
	Add(Equation* left, Equation* right) : left(left), right(right) {}

	
	Equation* derivative(std::string var)
	{
		return new Add(left->derivative(var), right->derivative(var));
	}


	int eval(std::string term)
	{
		return left->eval(term) + right->eval(term);
	}


	std::string eq_to_str()
	{
		return "(" + left->eq_to_str() + "+" + right->eq_to_str() + ")";
	}
	
	
	void print() 
	{
		std::cout << this->eq_to_str();
	}

	
	Equation* simplify_equation() 
	{
		if (left->eq_to_str() == "0") 
		{
			return right->simplify_equation();
		}
		if (right->eq_to_str() == "0") 
		{
			return left->simplify_equation();
		}
		return new Add(left->simplify_equation(), right->simplify_equation());
	}

	
	bool is_variable() 
	{
		return left->is_variable() || right->is_variable();
	}

	~Add() 
	{
		delete[] left;
		delete[] right;
	}
};


class Sub : public Equation 
{
private:
	Equation* left, *right;

public:
	Sub() : left(nullptr), right(nullptr) {}

	
	Sub(Equation* left, Equation* right) : left(left), right(right) {}

	
	int eval(std::string term)
	{
		return left->eval(term) - right->eval(term);
	}


	Equation* derivative(std::string var)
	{
		return new Sub(left->derivative(var), right->derivative(var));
	}


	std::string eq_to_str()
	{
		return "(" + left->eq_to_str() + "-" + right->eq_to_str() + ")";
	}


	void print()
	{
		std::cout << this->eq_to_str();
	}


	Equation* simplify_equation() 
	{
		if (left->eq_to_str() == right->eq_to_str()) 
		{
			return new Number(0);
		}
		else 
		{
			return new Sub(left->simplify_equation(), right->simplify_equation());;
		}
	}

	
	bool is_variable() 
	{
		return left->is_variable() || right->is_variable();
	}

	
	~Sub() {
		delete[] left;
		delete[] right;
	}
};


class Mul : public Equation 
{
private:
	Equation* left, *right;

public:
	Mul() : left(nullptr), right(nullptr) {}

	
	Mul(Equation* left, Equation* right) : left(left), right(right) {}

	
	int eval(std::string term)
	{
		return left->eval(term) * right->eval(term);
	}


	Equation* derivative(std::string var)
	{
		return new Add(new Mul(left->derivative(var), right), new Mul(left, right->derivative(var)));
	}


	std::string eq_to_str()
	{
		return "(" + left->eq_to_str() + "*" + right->eq_to_str() + ")";
	}


	void print()
	{
		std::cout << this->eq_to_str();
	}


	Equation* simplify_equation() 
	{
		if (left->eq_to_str() == "0" || right->eq_to_str() == "0") 
		{
			return new Number(0);
		}
		if (left->eq_to_str() == "1") 
		{
			return right->simplify_equation();
		}
		if (right->eq_to_str() == "1") 
		{
			return left->simplify_equation();
		}
		return new Mul(left->simplify_equation(), right->simplify_equation());;
	}

	
	bool is_variable() 
	{
		return left->is_variable() || right->is_variable();
	}

	
	~Mul() 
	{
		delete[] left;
		delete[] right;
	}
};


class Div : public Equation 
{
private:
	Equation* left, *right;

public:
	Div() : left(nullptr), right(nullptr) {}

	
	Div(Equation* left, Equation* right) : left(left), right(right) {}

	
	int eval(std::string term)
	{
		return left->eval(term) / right->eval(term);
	}


	Equation* derivative(std::string var)
	{
		return new Div(new Sub(new Mul(left->derivative(var), right), new Mul(left, right->derivative(var))), new Mul(right, right));
	}


	std::string eq_to_str()
	{
		return "(" + left->eq_to_str() + "/" + right->eq_to_str() + ")";
	}
	
	
	void print()
	{
		std::cout << this->eq_to_str();
	}

	
	Equation* simplify_equation() 
	{
		if (left->eq_to_str() == "0")
		{
			return new Number(0);
		}
		if (right->eq_to_str() == "0")
		{
			throw("Division by zero.");
		}
		if (left->eq_to_str() == right->eq_to_str())
		{
			return new Number(1);
		}
		return new Div(left->simplify_equation(), right->simplify_equation());
	}

	
	bool is_variable() 
	{
		return left->is_variable() || right->is_variable();
	}

	
	~Div() 
	{
		delete[] left;
		delete[] right;
	}
};


int get_priority(char symbol)
{
	char signs_str[] = "()+-*/";
	
	for (size_t priority = 0; signs_str[priority] != '\0'; priority++)
	{
		if (symbol == signs_str[priority])
			return priority / 2;
	}
	
	return -1;
}


std::string to_postfix(std::string infix_str)
{
	int priority = 0;
	std::stack <char> signs_stack;
	std::string postfix_str = "";
	bool last_isdigit = false;

	for (int i = 0; i < infix_str.length(); i++) 
	{
		priority = get_priority(infix_str[i]);
		
		if (priority == -1)
		{
			postfix_str = postfix_str + infix_str[i];
			last_isdigit = true;
			continue;
		}
		
		if (infix_str[i] == '(')
		{
			signs_stack.push('(');
			last_isdigit = false;
			continue;
		}

		if (infix_str[i] == ')')
		{
			while (signs_stack.top() != '(')
			{
				postfix_str = postfix_str + ' ' + signs_stack.top();
				signs_stack.pop();
			}
			signs_stack.pop();
			continue;
		}

		if (infix_str[i] == '-')
		{
			if (!last_isdigit) 
			{
				postfix_str += '0';
			}
		}
		
		while (!signs_stack.empty() && priority <= get_priority(signs_stack.top())) 
		{
			postfix_str = postfix_str + ' ' + signs_stack.top();
			signs_stack.pop();
		}
		
		postfix_str = postfix_str + ' ';
		signs_stack.push(infix_str[i]);
		last_isdigit = false;
	}

	while (!signs_stack.empty()) 
	{
		postfix_str = postfix_str + ' ' + signs_stack.top();
		signs_stack.pop();
	}

	return postfix_str;
}



Equation* split_into_equations(std::string infix_str) 
{
	std::string postfix_str = "";
	postfix_str = to_postfix(infix_str);

	std::string signs_stack = "-+*/";
	std::stack <Equation*> equations_stack;
	
	for (size_t i = 0; i < postfix_str.length(); i++) 
	{
		if (postfix_str[i] == ' ')
			continue;
		
		if (get_priority(postfix_str[i]) == -1) 
		{
			Equation* new_equation = nullptr;

			if (isdigit(postfix_str[i])) 
			{
				int number = 0;
				while (postfix_str[i] != ' ' && i < postfix_str.length()) 
				{
					number = number * 10 + (static_cast<int>(postfix_str[i++]) - static_cast<int>('0'));
				}
				new_equation = new Number(number);
			}
			
			else 
			{
				std::string variable = "";
				while (postfix_str[i] != ' ' && i < postfix_str.length()) 
				{
					variable += postfix_str[i++];
				}
				new_equation = new Variable(variable);
			}
			equations_stack.push(new_equation);
		}
		
		else
		{
			Equation *s_equation = equations_stack.top();
			equations_stack.pop();
			Equation* f_equation = equations_stack.top();
			equations_stack.pop();
			if (postfix_str[i] == '+')
				equations_stack.push(new Add(f_equation, s_equation));
			if (postfix_str[i] == '-')
				equations_stack.push(new Sub(f_equation, s_equation));
			if (postfix_str[i] == '*')
				equations_stack.push(new Mul(f_equation, s_equation));
			if (postfix_str[i] == '/')
			{
				if (s_equation->eq_to_str() == "0")
					throw("Division by zero.");
				equations_stack.push(new Div(f_equation, s_equation));
			}
		}
	}
	return equations_stack.top();
}


Equation* simplify(Equation *base) 
{
	if (!(base->is_variable())) 
	{
		return new Number(base->eval(""));
	}

	Equation* simplified = base->simplify_equation();

	if (simplified->eq_to_str() == base->eq_to_str()) 
	{
		return base;
	}

	return simplify(simplified);
}


int main() {
	std::string input_equation;
	std::cin >> input_equation;
	Equation *equation = split_into_equations(input_equation);
	equation->derivative("x")->print();
	//simplify(equation)->print();
	delete[] equation;
	return 0;
}