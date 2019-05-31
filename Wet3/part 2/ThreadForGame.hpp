#ifndef __THREADFORGAME_H
#define __THREADFORGAME_H
#include "Thread.hpp"
#include "PCQueue.hpp"

class ThreadForGame : public Thread {
private:
	PCQueue<vector<string>>* jobs;
	vector<string>* game_matrix;
	vector<string>* next_matrix;
	uint lines_for_thread;
	uint thread_num;
	uint cols;
	uint lines;
	uint* total_t_finish;

	pthread_mutex_t m;

public:
	//constacter
	ThreadForGame(uint i, PCQueue<vector<string>>* jobs, vector<string>* game_matrix, vector<string>* next_matrix, uint lines_for_thread,
		uint cols, uint lines, uint* total_t_finish) :
		Thread(i),
		jobs(jobs),
		game_matrix(game_matrix),
		next_matrix(next_matrix),
		lines_for_thread(lines_for_thread),
		thread_num(i),
		cols(cols),
		lines(lines),
		total_t_finish(total_t_finish)
	{
		pthread_mutex_init(&m, NULL);
	}


	void thread_workload() {
		while (true) {
			vector<string> temp = jobs->pop();
			cout << "thread pop the new game_matrix  " << endl; 

			for (uint row = 0; row < lines; row++) {
				for (uint col = 0; col < cols; col += 2) {
					if ((temp[row][col]) == '0' && num_live_neighbors( row, col) == 3) {
						(*next_matrix)[ row][col] = '1';
					}
					else if ((temp[row][col]) != '0' &&
						(num_live_neighbors( row, col) == 3 || num_live_neighbors( row, col) == 2)) {
						(*next_matrix)[row][col] = '1';
					}
					else {
						(*next_matrix)[row][col] = '0';
					}
				}
			}


			pthread_mutex_lock(&m);
			(*total_t_finish)++;
			pthread_mutex_unlock(&m);


		}
	}

private:
	uint num_live_neighbors(int row, int col) {
		uint total = 0;

		if (col - 2 >= 0 && (*game_matrix)[row][col - 2] != '0')
			total++;
		if (col + 2 < cols && (*game_matrix)[row][col + 2] != '0')
			total++;
		if (row - 1 >= 0 && (*game_matrix)[row - 1][col] != '0')
			total++;
		if (row + 1 < lines && (*game_matrix)[row + 1][col] != '0')
			total++;
		if (col - 2 >= 0 && row - 1 >= 0 && (*game_matrix)[row-1][col - 2] != '0')
			total++;
		if (col - 2 >= 0 && row + 1 < lines && (*game_matrix)[row+1][col - 2] != '0')
			total++;
		if (col + 2 < cols && row - 1 >= 0 && (*game_matrix)[row - 1][col+2] != '0')
			total++;
		if (col + 2 < cols && row + 1 < lines && (*game_matrix)[row + 1][col+2] != '0')
			total++;

		return total;
	}
};

#endif

