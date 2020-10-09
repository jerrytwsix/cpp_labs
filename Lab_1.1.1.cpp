#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>

class Matrix
{
private:
	
	size_t size;
	int **arr;
	friend class Row;
	friend class Column;
	friend std::istream& operator >> (std::istream& ost, const Matrix& matrix);
	friend std::ostream& operator << (std::ostream& ost, const Matrix& matrix);


	class Row
	{	
		size_t row_num;
		Matrix &matrix;
	public:
		Row(Matrix& matrix, uint32_t num): matrix(matrix), row_num(num) {}

		int& operator[](uint32_t num)
		{
			if (num > matrix.size)
			{
				throw("Matrix sizes don't fit while using operator ()[]");
			}
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
			if (num > matrix.size)
			{
				throw("Matrix sizes don't fit while using operator [][]");
			}
			return matrix.arr[num][column_num];
		}
	};
	void alloc(size_t size)
	{
		this->size = size;
		arr = new int*[size];
		for (size_t i = 0; i < size; ++i)
		{
			arr[i] = new int[size];
			memset(arr[i], 0, size * sizeof(int));
		}
	}

public:
	
	
	Matrix() : size(0), arr(nullptr) {}

	
	Matrix(size_t size)
	{
		if (size < 0)
		{
			throw("Matrix size is less than 0");
		}
		alloc(size);
	}
	
	
	Matrix(size_t size, int *diag_arr) 
	{
		if (size < 0)
		{
			throw("Matrix size is less than 0");
		}
		alloc(size);
		for (size_t i = 0; i < size; ++i)
		{
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
		if (size != that.size)
		{
			throw("Matrix sizes don't fit while using operator +");;
		}
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
		if (size != that.size)
		{
			throw("Matrix sizes don't fit while using operator *");;
		}
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
		if (size != that.size)
		{
			throw("Matrix sizes don't fit while using operator ==");;
		}
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
		return !(this == &that);
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
		if (row >= size || column >= size)
		{
			throw("Matrix sizes don't fit while using operator ()");
		}
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
		if (row_num >= size)
		{
			throw("Matrix sizes don't fit while using operator []");
		}
		Row row(*this, row_num);
		return row;
	}


	Column operator ()(uint32_t column_num)
	{
		if (column_num >= size)
		{
			throw("Matrix sizes don't fit while using operator ()");
		}
		Column column(*this, column_num);
		return column;
	}


	~Matrix()
	{
		for (size_t i = 0; i < size; ++i)
		{
			delete[] arr[i];
		}
		delete[] arr;
	}

};


std::istream& operator >> (std::istream& ost, const Matrix& matrix)
{
	for (size_t i = 0; i < matrix.size; i++)
	{
		for (size_t j = 0; j < matrix.size; j++)
		{
			ost >> matrix.arr[i][j];
		}
	}
	return ost;
}


std::ostream& operator << (std::ostream& ost, const Matrix& matrix)
{
	for (size_t i = 0; i < matrix.size; i++)
	{
		for (size_t j = 0; j < matrix.size; j++)
		{
			ost << matrix.arr[i][j] << ' ';
		}
		ost << std::endl;
	}
	return ost;
}


int main() 
{
	int N, k;
	std::cin >> N >> k;
	int *arr = new int[N];
	for (size_t i = 0; i < N; i++)
		arr[i] = k;
	Matrix A(N), B(N), C(N), D(N), K(N, arr), result(N);
	std::cin >> A;
	std::cin >> B;
	std::cin >> C;
	std::cin >> D;
	result = (A + B * !C + K) * !D;
	std::cout << result;
	delete[] arr;
	return 0;
}