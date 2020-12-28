#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <fstream>
#include <vector>
#include <set>
#include <queue>
#include <regex>

using namespace std;


class Crawler
{
public:
	Crawler(string _first_filename, size_t _thread_count) : first_filename(_first_filename), thread_count(_thread_count), links_counter(0), time_elapsed(0)
	{
		ifstream input;
		string first_link;
		filenames.insert(first_filename);
		input.open(first_filename);
		while (input)
		{
			getline(input, first_link);
		}
		input.close();
		links.push(first_link);
	}


	void parse()
	{
		auto time_start = chrono::high_resolution_clock::now();

		for (size_t i = 0; i < thread_count; ++i)
		{
			threads.emplace_back(&Crawler::threadParser, ref(*this));
		}

		for (auto& t : threads)
		{
			t.join();
		}

		auto time_finish = chrono::high_resolution_clock::now();

		time_elapsed = time_finish - time_start;
	}


	size_t get_links_amount()
	{
		return links_counter;
	}


	chrono::duration<double> get_time_elapsed()
	{
		return time_elapsed;
	}


private:
	string first_filename;
	size_t thread_count, links_counter;
	vector <thread> threads;
	queue <string> links;
	set <string> filenames;
	mutex thread_protector;
	atomic <int> workers;
	chrono::duration<double> time_elapsed;


	void threadParser()
	{
		ifstream input;
		string curr_link, tmp_link, tmp_filename, push_link;
		regex link_mask("(<a href=\"file://[a-za-z0-9.]+html\">)");
		regex file_mask("([a-za-z0-9.]+html)");
		smatch match, file_match;

		while (true)
		{
			unique_lock <mutex> locker(thread_protector);

			if (!links.empty())
			{
				curr_link = links.front();
				links.pop();
				++links_counter;
				locker.unlock();

				workers++;
				while (regex_search(curr_link, match, link_mask))
				{
					tmp_filename = match.str();
					regex_search(tmp_filename, file_match, file_mask);
					curr_link = match.suffix().str();
					tmp_filename = file_match.str();
					locker.lock();
					if (!filenames.count(tmp_filename))
					{
						push_link = { 0 };
						input.open("test_data/" + tmp_filename);

						while (getline(input, tmp_link))
						{
							push_link += tmp_link;
						}

						input.close();
						filenames.insert(tmp_filename);
						links.push(push_link);
					}
					locker.unlock();
				}
				workers--;
			}
			else
			{
				locker.unlock();
				while (links.empty())
				{
					if (!workers)
					{
						return;
					}
				}
			}
		}
	}
};

int main()
{
	ifstream input;
	ofstream output;
	input.open("input.txt");
	output.open("output.txt");
	string filename;
	int thread_count = 0;

	input >> filename >> thread_count;
	input.close();

	Crawler crawler(filename, thread_count);
	crawler.parse();

	output << crawler.get_links_amount() << " " << chrono::duration_cast<std::chrono::seconds>(crawler.get_time_elapsed()).count() << endl;
	output.close();

	return 0;
}