#ifndef __THREADFORGAME_H
#define __THREADFORGAME_H
#include "Thread.hpp"
#include "PCQueue.hpp"


class ThreadForGame : public Thread {
private:
	PCQueue<job> *jobs;
	vector<string> *game_matrix;
	vector<string> *next_matrix;
	pthread_mutex_t *m;
	int cols;
	int lines;
	uint *total_t_finish;
	uint *m_gen_num;
	uint *cur_gen_num;
	vector<tile_record> *m_tile_hist;

public:
	//constacter
	ThreadForGame(uint i, PCQueue<job>* jobs, vector<string>* game_matrix, vector<string>* next_matrix, pthread_mutex_t* m,
		uint cols, uint lines, uint* total_t_finish, uint* m_gen_num, uint* cur_gen_num, vector<tile_record>* m_tile_hist) :
		Thread(i),
		jobs(jobs),
		game_matrix(game_matrix),
		next_matrix(next_matrix),
		m(m),
		cols(cols),
		lines(lines),
		total_t_finish(total_t_finish),
		m_gen_num(m_gen_num),
		cur_gen_num(cur_gen_num),
		m_tile_hist(m_tile_hist)
	{}


	void thread_workload() {
		while (*cur_gen_num < *m_gen_num -1) {
			job new_job = jobs->pop();
			int original_row = new_job.original_start_row;
			int total_rows = new_job.total_rows;
			int num_of_live_neighbors = 0;

			//execute job
			auto tile_start = std::chrono::system_clock::now(); //start timer
			for (int row = 0; row < total_rows; row++) {
				for (int col = 0; col < cols; col += 2) {
					num_of_live_neighbors = num_live_neighbors(original_row + row, col);
					if ((*game_matrix)[original_row + row][col] == '0' && num_of_live_neighbors == 3) {
						(*next_matrix)[original_row + row][col] = '1';
					}
					else if ((*game_matrix)[original_row + row][col] == '1' &&
						(num_of_live_neighbors == 3 || num_of_live_neighbors == 2)) {
						(*next_matrix)[original_row + row][col] = '1';
					}
					else {
						(*next_matrix)[original_row + row][col] = '0';
					}
				}
			}
			auto tile_end = std::chrono::system_clock::now(); // stop timer

			//create tile_record
			tile_record *record = new tile_record();
			record->thread_id = m_thread_id;
			record->tile_compute_time = (double)std::chrono::duration_cast<std::chrono::microseconds>(tile_end - tile_start).count();

			//push tile_record to m_tile_hist and increase total_t_finish
			pthread_mutex_lock(m);
			m_tile_hist->push_back(*record);
			(*total_t_finish)++;
			pthread_mutex_unlock(m);

		}
		return;
	}

private:
	//function to calculate total live neighbors for cell (row,col)
	uint num_live_neighbors(int row, int col) {
		uint total = 0;

		if (col - 2 >= 0 && (*game_matrix)[row][col - 2] == '1')
			total++;
		if (col + 2 < cols && (*game_matrix)[row][col + 2] == '1')
			total++;
		if (row - 1 >= 0 && (*game_matrix)[row - 1][col] == '1')
			total++;
		if (row + 1 < lines && (*game_matrix)[row + 1][col] == '1')
			total++;
		if (col - 2 >= 0 && row - 1 >= 0 && (*game_matrix)[row-1][col - 2] == '1')
			total++;
		if (col - 2 >= 0 && row + 1 < lines && (*game_matrix)[row+1][col - 2] == '1')
			total++;
		if (col + 2 < cols && row - 1 >= 0 && (*game_matrix)[row - 1][col+2] == '1')
			total++;
		if (col + 2 < cols && row + 1 < lines && (*game_matrix)[row + 1][col+2] == '1')
			total++;

		return total;
	}
};

#endif
