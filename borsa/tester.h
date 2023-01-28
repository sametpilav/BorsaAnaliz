//
//  tester.h
//  BorsaAnaliz2
//
//  Created by Samet Pilav on 2.05.2021.
//

#ifndef tester_h
#define tester_h

#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <string>

namespace ba {

	namespace tester {
		
		template<typename StrategyType>
		static
		std::vector<ba::TestResultSameParameterMultiStock> RunTestWithSameParamsOnEveryStock(std::map<std::string, std::vector<ba::Bar>> tickerNameToBarsMap, const std::string& output_file_name) {
			
			std::vector<ba::TestResultSameParameterMultiStock> testResults;
			
			constexpr char TAB = ';';
			constexpr char ENDL = '\n';
			
			std::ostringstream report;
			report.imbue(std::locale(report.getloc(), new std::numpunct_byname<char>("de_DE.utf8")));
			
			report << TAB;
			for (double stoploss_percentage_to_sell = 0.1; stoploss_percentage_to_sell < 30.0; stoploss_percentage_to_sell += 0.1) {
				report << stoploss_percentage_to_sell << TAB;
			}
			report << ENDL;
			
			for (double stoploss_percentage_to_buy = 0.1; stoploss_percentage_to_buy < 30.0; stoploss_percentage_to_buy += 0.1) {
				report << stoploss_percentage_to_buy << TAB;
				
				for (double stoploss_percentage_to_sell = 0.1; stoploss_percentage_to_sell < 30.0; stoploss_percentage_to_sell += 0.1) {
					
					ba::Money sum_of_total_balances = 0;
					
					const size_t vector_size = ba::CollectionUtils::getFirst(tickerNameToBarsMap).value().second.size();
					std::vector<ba::Money> gain_history(vector_size, ba::Money{});
					
					for (const auto& [ticker_name, bars] : tickerNameToBarsMap) {
						
						StrategyType strategy(stoploss_percentage_to_buy, stoploss_percentage_to_sell);
						const ba::Money balance = 10'000.00;
						
						const ba::Ticker tahta( bars );
						const ba::OrderLogger logger = tahta.RunTest(strategy, balance);
						
						const std::vector<ba::Money>& bar_end_net_worths = logger.bar_end_net_worths;
						
						for (size_t i = 0; i < gain_history.size(); i++) {
							gain_history[i] += bar_end_net_worths[i];
						}
						
						const ba::Money final_balance = ba::CollectionUtils::getLast(bar_end_net_worths).value();
						sum_of_total_balances += final_balance;
						
					}
					
					const double gain = sum_of_total_balances / (10000 * tickerNameToBarsMap.size());
					report << gain << TAB;
					
					std::for_each(gain_history.begin(), gain_history.end(), [&tickerNameToBarsMap](ba::Money& net_worth){
						net_worth /= ba::Money{10000} * tickerNameToBarsMap.size();
					});
					
					ba::TestResultSameParameterMultiStock testResult;
					testResult.stoploss_percentage_to_buy = stoploss_percentage_to_buy;
					testResult.stoploss_percentage_to_sell = stoploss_percentage_to_sell;
					testResult.gain_history = gain_history;
					
					testResults.push_back(testResult);
				}
				
				report << ENDL;
			}
			
			if (output_file_name != "") {
				std::ofstream of(output_file_name);
				of << report.str();
			}
			
			return testResults;
		}
		
	}

}


#endif /* tester_h */
