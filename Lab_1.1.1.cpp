#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

class Matrix
{
private:
	
	size_t size;
	int **arr;
	friend class Row;
	friend class Column;


	class Row
	{	
		size_t row_num;
		Matrix &matrix;
	public:
		Row(Matrix& matrix, uint32_t num): matrix(matrix), row_num(num) {}

		int& operator[](uint32_t num)
		{
			assert(num < matrix.size);
			return matrix.arr[row_num][num];
		}
	};


	class Column
	{
		size_t column_num;
		Matrix &matrix;
	public:
		Column(Matrix& matrix, uint32_t num) : matrix(matrix), column_num(num) {}

		int& operator[](uint32_t num)
		{
			assert(num < matrix.size);
			return matrix.arr[num][column_num];
		}
	};


public:
	
	
	Matrix() : size(0)
	{
		arr = NULL;
	}

	
	Matrix(size_t size)
	{
		assert(size >= 0);
		this->size = size;
		arr = new int*[size];
		for (size_t i = 0; i < size; ++i)
		{
			arr[i] = new int[size];
			memset(arr[i], 0, size * sizeof(int));
		}
	}
	
	
	Matrix(size_t size, int *diag_arr) 
	{
		//assert(size >= 0 && sizeof(diag_arr)/sizeof(int) == size); // sizeof(diag_arr) - ptr to the first elem of array, no way
		assert(size >= 0);
		this->size = size;
		arr = new int*[size];
		for (size_t i = 0; i < size; ++i)
		{
			arr[i] = new int[size];
			memset(arr[i], 0, size * sizeof(int));
			arr[i][i] = diag_arr[i];
		}
	}

	
	Matrix(const Matrix& that): size(that.size)
	{
		
		arr = new int*[size];
		for (size_t i = 0; i < size; ++i)
		{
			arr[i] = new int[size];
			for (size_t j = 0; j < size; ++j)
				arr[i][j] = that.arr[i][j];
		}
	}


	void inputFromFile(char* filename)
	{
		std::ifstream fin(filename);
		for (size_t i = 0; i < size; i++)
		{
			for (size_t j = 0; j < size; j++)
			{
				fin >> arr[i][j];
			}
		}
		fin.close();
	}
	
	
	void outputToFile(char* filename)
	{
		std::ofstream fout(filename);
		for (size_t i = 0; i < size; i++)
		{
			for (size_t j = 0; j < size; j++)
			{
				fout << arr[i][j] << " ";
			}
			fout << std::endl;
		}
		fout.close();
	}

	
	void consoleInput()
	{
		for (size_t i = 0; i < size; i++)
		{
			for (size_t j = 0; j < size; j++)
			{
				std::cin >> arr[i][j];
			}
		}
	}

	
	void consoleOutput()
	{
		for (size_t i = 0; i < size; i++)
		{
			for (size_t j = 0; j < size; j++)
			{
				std::cout << arr[i][j] << ' ';
			}
			std::cout << std::endl;
		}
	}

	
	Matrix operator=(const Matrix& that)
	{
		if (this != &that)
		{
			delete[] arr;
			size = that.size;
			arr = new int*[size];
			for (size_t i = 0; i < size; ++i)
			{
				arr[i] = new int[size];
				for (size_t j = 0; j < size; ++j)
					arr[i][j] = that.arr[i][j];
			}
		}
		return *this;
	}


	Matrix operator+(const Matrix& that)
	{
		assert(size == that.size);
		Matrix result(size);
		for (size_t i = 0; i < size; ++i)
		{
			for (size_t j = 0; j < size; ++j)
			{
				result.arr[i][j] = arr[i][j] + that.arr[i][j];
			}
		}
		return result;
	}

	
	Matrix operator*(const Matrix& that)
	{
		assert(size == that.size);
		Matrix result(size);
		for (size_t i = 0; i < size; ++i)
		{
			for (size_t j = 0; j < size; ++j)
			{
				for (size_t k = 0; k < size; ++k)

				{
					result.arr[i][j] += arr[i][k] * that.arr[k][j];
				}
			}
		}
		return result;
	}
	
	
	bool operator ==(const Matrix& that) 
	{
		assert(size == that.size);
		for (size_t i = 0; i < size; ++i)
		{
			for (size_t j = 0; j < size; ++j)
			{
				if (arr[i][j] != that.arr[i][j])
				{
					return false;
				}
			}
		}
		return true;
	}
	
	
	bool operator !=(const Matrix& that) 
	{
		if (this == &that) 
		{
			return false;
		}
		return true;
	}
	
	
	Matrix operator ! () //transpose operator
	{ 
		Matrix result(size);
		for (size_t i = 0; i < size; ++i)
		{
			for (size_t j = 0; j < size; ++j)
			{
				result.arr[i][j] = arr[j][i];
			}
		}
		return result;
	}

	
	Matrix operator ()(uint32_t row, uint32_t column)
	{
		assert(row < size, column < size);
		row--;
		column--;
		Matrix result(size - 1);
		for (size_t i = 0, ik = 0; i < size; ++i)
		{
			if (i == row) { continue; }
			for (size_t j = 0, jk = 0; j < size; ++j)
			{
				if (j == column) { continue; }
				result.arr[ik][jk] = arr[i][j];
				jk++;
			}
			ik++;
		}
		return result;
	}

	
	Row operator [](uint32_t row_num)
	{
		assert(row_num < size);
		Row row(*this, row_num);
		return row;
	}


	Column operator ()(uint32_t column_num)
	{
		assert(column_num < size);
		Column column(*this, column_num);
		return column;
	}


	~Matrix()
	{
		delete[] arr;
	}


};


int main() 
{
	int N, k;
	std::cin >> N >> k;
	int *arr = new int[N];
	for (size_t i = 0; i < N; i++)
		arr[i] = k;
	Matrix A(N), B(N), C(N), D(N), K(N, arr), result(N);
	A.consoleInput();
	B.consoleInput();
	C.consoleInput();
	D.consoleInput();
	result = (A + B * !C + K) * !D;
	result.consoleOutput();
	delete[] arr;
	return 0;
}