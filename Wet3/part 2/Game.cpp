#include "Game.hpp"

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/


Game::Game(game_params param) 
{
	m_gen_num = param.n_gen;
	lines = utils::num_lines(param.filename);
	cols = utils::num_cols(param.filename);
	m_thread_num = std::min(param.n_thread, lines);
	interactive_on = param.interactive_on;
	print_on = param.print_on;
	filename = param.filename;
	lines_for_thread = lines / m_thread_num;
	total_t_finish = 0;
	pthread_mutex_init(&m, NULL);
	cur_gen_num = 0;
}

Game::~Game() {}

void Game::run() {

	_init_game(); // Starts the threads and all other variables you need
	print_board("Initial Board");
	for (uint i = 0; i < m_gen_num; ++i) {
		auto gen_start = std::chrono::system_clock::now();
		_step(i); // Iterates a single generation 
		auto gen_end = std::chrono::system_clock::now();
		m_gen_hist.push_back((double)std::chrono::duration_cast<std::chrono::microseconds>(gen_end - gen_start).count());
		print_board(nullptr);
	} // generation loop
	print_board("Final Board");
	_destroy_game();
}

const vector<double> Game::gen_hist() const {
	return m_gen_hist;
}

const vector<tile_record> Game::tile_hist() const {
	return m_tile_hist;
}

uint Game::thread_num() const {
	return m_thread_num;
}

void Game::_init_game() {

	// Create game fields - Consider using utils:read_file, utils::split
	game_matrix = new vector<string>(utils::read_lines(filename));
	next_matrix = new vector<string>(utils::read_lines(filename));


	// Create & Start threads
	ThreadForGame* t;
	for (uint i = 0; i < m_thread_num; i++) {
		t = new ThreadForGame(i, &jobs, game_matrix,next_matrix,&m,cols,lines, &total_t_finish,&m_gen_num,&cur_gen_num,&m_tile_hist);
		m_threadpool.push_back(t);
		t->start();
	}

	// Testing of your implementation will presume all threads are started here
}

void Game::_step(uint curr_gen) {
	
	uint i, total_row;
	cur_gen_num = curr_gen;

	//creat the work for each thred
	for (i = 0; i < m_thread_num; i++) {	
		total_row = lines_for_thread;
		//if this is the lest thread we need to add the remeinder
		if (i == m_thread_num - 1) {
			total_row += lines - (m_thread_num * lines_for_thread);
		}

		//Push job to the queue
		job new_job ;
		new_job.original_start_row = i* lines_for_thread;
		new_job.total_rows = total_row;
		jobs.push(new_job);	
	}


	// Wait for the workers to finish calculating 
	while (total_t_finish != m_thread_num) {}

	// Swap pointers between current and next field 
	*game_matrix = *next_matrix;

	total_t_finish = 0;
	// NOTE: Threads must not be started here - doing so will lead to a heavy penalty in your grade 

}

void Game::_destroy_game(){
	// Destroys board and frees all threads and resources
	// Not implemented in the Game's destructor for testing purposes. 
	// All threads must be joined here
	for (uint i = 0; i < m_thread_num; i++) {
        m_threadpool[i]->join();
		delete (m_threadpool[i]);
    }
	delete game_matrix;
	delete next_matrix;
	pthread_mutex_destroy(&m);
}

/*--------------------------------------------------------------------------------
								
--------------------------------------------------------------------------------*/
inline void Game::print_board(const char* header) {

	if(print_on){ 

		// Clear the screen, to create a running animation 
		if(interactive_on)
			system("clear");

		// Print small header if needed
		if (header != nullptr)
			cout << "<------------" << header << "------------>" << endl;
		
		uint length = (cols + 1) / 2;
		//Print the board
		cout << u8"╔" << string(u8"═") * length << u8"╗" << endl;
		for (uint i = 0; i < lines; i++) {
			cout << u8"║";
			for (uint j = 0; j < cols; j+=2) {
				cout << ((*game_matrix)[i][j] == '1' ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * length << u8"╝" << endl;

		// Display for GEN_SLEEP_USEC micro-seconds on screen 
		if(interactive_on)
			usleep(GEN_SLEEP_USEC);
	}

}


/* Function sketch to use for printing the board. You will need to decide its placement and how exactly 
	to bring in the field's parameters. 

		cout << u8"╔" << string(u8"═") * field_width << u8"╗" << endl;
		for (uint i = 0; i < field_height ++i) {
			cout << u8"║";
			for (uint j = 0; j < field_width; ++j) {
				cout << (field[i][j] ? u8"█" : u8"░");
			}
			cout << u8"║" << endl;
		}
		cout << u8"╚" << string(u8"═") * field_width << u8"╝" << endl;
*/ 
