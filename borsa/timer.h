//
//  timer.h
//  BorsaAnaliz2
//
//  Created by Samet Pilav on 24.04.2021.
//

#ifndef timer_h
#define timer_h

namespace ba {

	class Timer {
		
	private:
		std::chrono::steady_clock::time_point begin;
		
	public:
		Timer() : begin(std::chrono::steady_clock::now()) {}
		
		~Timer() {
			std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
			std::cout << "\n";
			std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]\n";
			std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count() << "[µs]\n";
			std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::nanoseconds> (end - begin).count() << "[ns]\n";
		}
	};

}

#endif /* timer_h */
