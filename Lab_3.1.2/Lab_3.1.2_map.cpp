#include <iostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

template <typename K, typename V>
class HashMap;

template <typename K, typename V>
class Pair
{
	K _key;
	V _value;
	bool _free;
	bool _nondelete;
	friend class HashMap<K, V>;


	Pair() : _free(true), _nondelete(false){}


	Pair(K _key, V _value) : _key(_key), _value(_value), _free(true), _nondelete(false){}
};


template <typename K, typename V>
class HashMap
{
public:
	HashMap() : block_size(1), overflow_koef(0.75), size(0), size_non_null(0)
	{
		items = new Pair<K, V>[block_size];
	};


	HashMap(size_t size) : block_size(size), overflow_koef(0.75), size(0), size_non_null(0)
	{
		items = new Pair<K, V>[block_size];
	};


	void insert(const K key, const V value)
	{
		size_t hash_value = get_hash(key);
		while(!items[hash_value]._free && items[hash_value]._key != key)
		{
			if (hash_value != block_size - 1)
				++hash_value;
			else hash_value = 0;
		}
		if (items[hash_value]._key == key && items[hash_value]._nondelete)
		{
			items[hash_value]._value = value;
			return;
		}
		items[hash_value]._key = key;
		items[hash_value]._value = value;
		items[hash_value]._free = false;
		items[hash_value]._nondelete = true;
		++size;
		++size_non_null;
		if (static_cast<double>(size) / static_cast<double>(block_size) > overflow_koef)
			rehash();
	}


	void erase(const K key)
	{
		size_t hash_value = get_hash(key);
		while (items[hash_value]._key != key)
		{
			if (items[hash_value]._free)
				return;
			if (hash_value != block_size - 1)
				++hash_value;
			else hash_value = 0;
		}
		if (!items[hash_value]._nondelete) return;
		items[hash_value]._nondelete = false;
		if(size_non_null != 0)
			size_non_null--;
	}


	V find(const K key)
	{
		size_t hash_value = get_hash(key);
		while(!items[hash_value]._free && items[hash_value]._key != key)
		{
			if (hash_value != block_size - 1)
				++hash_value;
			else hash_value = 0;
		}
		if(!items[hash_value]._nondelete) throw("rrer");
		return(items[hash_value]._value);
	}


	size_t get_size()
	{
		return size_non_null;
	}
	
	
	size_t get_amount_unique()
	{
		if (size_non_null == 0)
			return 0;
		unique_values.clear();
		for (size_t i = 0; i < block_size && unique_values.size() != size_non_null; ++i)
		{
			if (!items[i]._free && items[i]._nondelete)
			{
				unique_values.insert(items[i]._value);
			}
		}
		return unique_values.size();
	}


	~HashMap()
	{
		delete[] items;
	}
private:
	Pair <K, V> *items = nullptr;
	float overflow_koef;
	size_t block_size;
	size_t size;
	size_t size_non_null;
	set <V> unique_values;

	class Iterator
	{
		Pair <K, V> *pair_ptr;
		K pair_key;
		V pair_value;
		friend class HashMap<K, V>;

		Iterator(Pair<K, V> *pair_ptr) : pair_ptr(pair_ptr)
		{
			pair_key = pair_ptr->_key;
			pair_value = pair_ptr->_value;
		}


		Iterator& operator =(Iterator &that)
		{
			if (this != &that)
			{
				pair_ptr = that.pair_ptr;
				pair_key = that.pair_key;
				pair_value = that.pair_value;
			}
			return *this;
		}


		bool operator ==(Iterator &that)
		{
			return (pair_ptr == that.pair_ptr);
		}


		bool operator !=(Iterator &that)
		{
			return !(pair_ptr == that.pair_ptr);
		}


		void operator++()
		{
			do
			{
				++pair_ptr;
			} while (pair_ptr->_free);
			pair_key = pair_ptr->_key;
			pair_value = pair_ptr->_value;
		}


		void operator++(int n)
		{
			do
			{
				pair_ptr++;
			} while (pair_ptr->_free);
			pair_key = pair_ptr->_key;
			pair_value = pair_ptr->_value;
		}
	};


	Iterator begin()
	{
		size_t i = 0;
		while (items[i]._free) ++i;
		Iterator iter(&items[i]);
		return iter;
	}


	Iterator end()
	{
		size_t i = block_size - 1;
		while (items[i]._free) --i;
		Iterator iter(&items[i]);
		return iter;
	}


	size_t get_hash(K key)
	{
		hash<K> hash_func;
		return hash_func(key) % block_size;
	}


	void rehash()
	{
		HashMap new_map(block_size * 2);
		for(auto i = begin(), e = end(); ;i++)
		{
			new_map.insert(i.pair_key, i.pair_value);
			if (i == e)
				break;
		}
		swap(items, new_map.items);
		block_size *= 2;
	}
};


template <typename K, typename V>
class MultiHashMap
{
public:
	MultiHashMap() : block_size(1), overflow_koef(0.7), size(0), size_non_null(0)
	{
		items = new Pair<K, V>[block_size];
	};


	MultiHashMap(size_t size) : block_size(size), overflow_koef(0.7), size(0), size_non_null(0)
	{
		items = new Pair<K, V>[block_size];
	};


	void insert(const K key, const V value)
	{
		size_t hash_value = get_hash(key);
		while (!items[hash_value]._free)
		{
			if (items[hash_value]._key == key && !items[hash_value]._nondelete)
			{
				break;
			}
			if (hash_value != block_size - 1)
				++hash_value;
			else hash_value = 0;
		}
		items[hash_value]._key = key;
		items[hash_value]._value = value;
		items[hash_value]._free = false;
		items[hash_value]._nondelete = true;
		++size;
		++size_non_null;
		if (static_cast<float>(size / block_size) > overflow_koef)
			rehash();
	}


	void erase(const K key)
	{
		size_t hash_value = get_hash(key);
		while (!items[hash_value]._free)
		{
			if (items[hash_value]._key == key && items[hash_value]._nondelete)
			{
				items[hash_value]._nondelete = false;
				if(size_non_null != 0)
					size_non_null--;
			}
			if (hash_value != block_size - 1)
				++hash_value;
			else hash_value = 0;
		}
	}


	V find(const K key)
	{
		size_t hash_value = get_hash(key);
		while (!items[hash_value]._free && items[hash_value]._key != key)
		{
			if (hash_value != block_size - 1)
				++hash_value;
			else hash_value = 0;
		}
		if (items[hash_value]._free) throw("No key found in hashMap");
		if (!items[hash_value]._nondelete) throw("Searching key is delete");
		return(items[hash_value]._value);
	}


	size_t get_size()
	{
		return size_non_null;
	}


	size_t get_amount_unique()
	{
		unique_values.clear();
		for (size_t i = 0; i < size && unique_values.size() != size_non_null; ++i)
		{
			if (!items[i]._free && items[i]._nondelete)
			{
				unique_values.insert(items[i]._value);
			}
		}
		return unique_values.size();
	}


	size_t get_amount_by_key(K key)
	{
		size_t amount = 0;
		size_t hash_value = get_hash(key);
		while (!items[hash_value]._free)
		{
			if (items[hash_value]._key == key && items[hash_value]._nondelete)
			{
				amount++;
			}
			if (hash_value != block_size - 1)
				++hash_value;
			else hash_value = 0;
		}
		return amount;
	}


	vector get_elements_by_key(K key)
	{
		vector<V> elem_vect;
		size_t hash_value = get_hash(key);
		while (!items[hash_value]._free)
		{
			if (items[hash_value]._key == key && items[hash_value]._nondelete)
			{
				elem_vect.push_back(items[hash_value]._value);
			}
			if (hash_value != block_size - 1)
				++hash_value;
			else hash_value = 0;
		}
		return elem_vect;
	}

	~MultiHashMap()
	{
		delete[] items;
	}
private:
	Pair <K, V> *items = nullptr;
	float overflow_koef;
	size_t block_size;
	size_t size;
	size_t size_non_null;
	set <V> unique_values;

	class Iterator
	{
		Pair <K, V> *pair_ptr;
		K pair_key;
		V pair_value;
		friend class MultiHashMap<K, V>;

		Iterator(Pair<K, V> *pair_ptr) : pair_ptr(pair_ptr)
		{
			pair_key = pair_ptr->_key;
			pair_value = pair_ptr->_value;
		}


		Iterator& operator =(Iterator &that)
		{
			if (this != &that)
			{
				pair_ptr = that.pair_ptr;
				pair_key = that.pair_key;
				pair_value = that.pair_value;
			}
			return *this;
		}


		bool operator ==(Iterator &that)
		{
			return (pair_ptr == that.pair_ptr);
		}


		void operator++()
		{
			do
			{
				++pair_ptr;
			} while (pair_ptr->_free);
			pair_key = pair_ptr->_key;
			pair_value = pair_ptr->_value;
		}


		void operator++(int n)
		{
			do
			{
				pair_ptr++;
			} while (pair_ptr->_free);
			pair_key = pair_ptr->_key;
			pair_value = pair_ptr->_value;
		}
	};


	Iterator begin()
	{
		size_t i = 0;
		while (items[i]._free && !items[i]._nondelete) ++i;
		Iterator iter(&items[i]);
		return iter;
	}


	Iterator end()
	{
		size_t i = block_size - 1;
		while (items[i]._free && !items[i]._nondelete) --i;
		Iterator iter(&items[i]);
		return iter;
	}


	size_t get_hash(K key)
	{
		hash<K> hash_func;
		return hash_func(key) % block_size;
	}


	void rehash()
	{
		HashMap new_map(block_size * 2);
		for (auto i = begin(), e = end(); ; i++)
		{
			new_map.insert(i.pair_key, i.pair_value);
			if (i == e)
				break;
		}
		swap(items, new_map.items);
		block_size *= 2;
	}
};


template <typename K, typename V>
void task_hashmap()
{
	K key;
	V value;
	char sym;
	int n;
	std::cin >> n;
	HashMap<K, V> hash;
	for (int i = 0; i < n; i++)
	{
		cin >> sym;
		if (sym == 'A')
		{
			cin >> key >> value;
			hash.insert(key, value);
		}
		else
		{
			std::cin >> key;
			hash.erase(key);
		}
	}
	std::cout << hash.get_size() << ' ' << hash.get_amount_unique();
}


template<typename K>
void temp_v(char v_type)
{
	if (v_type == 'I')
		task_hashmap<K, int>();
	if (v_type == 'S')
		task_hashmap<K, string>();
	if (v_type == 'D')
		task_hashmap<K, double>();
}


int main()
{
	char k_type, v_type;
	std::cin >> k_type >> v_type;
	if (k_type == 'I')
		temp_v<int>(v_type);
	if (k_type == 'D')
		temp_v<double>(v_type);
	if (k_type == 'S')
		temp_v<string>(v_type);
	return 0;
}