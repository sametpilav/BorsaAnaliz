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
		std::vector<TestResultSameParameterMultiStock> RunTestWithSameParamsOnEveryStock(
			const std::map<std::string, std::vector<Bar>>& tickerNameToBarsMap,
			const std::string& output_file_name,
			const MoneyType balance)
		{
			
			std::vector<TestResultSameParameterMultiStock> testResults;
			
			constexpr char TAB = ';';
			constexpr char ENDL = '\n';
			
			std::ostringstream report;
			report.imbue(std::locale("de_DE"));
			
			report << TAB;
			for (double stoploss_percentage_to_sell = 0.1; stoploss_percentage_to_sell < 15.0; stoploss_percentage_to_sell += 0.2) {
				report << stoploss_percentage_to_sell << TAB;
			}
			report << ENDL;
			
			for (double stoploss_percentage_to_buy = 0.1; stoploss_percentage_to_buy < 15.0; stoploss_percentage_to_buy += 0.2) {
				report << stoploss_percentage_to_buy << TAB;
				
				for (double stoploss_percentage_to_sell = 0.1; stoploss_percentage_to_sell < 15.0; stoploss_percentage_to_sell += 0.2) {
					
					MoneyType sum_of_total_balances = 0;
					
					const size_t vector_size = CollectionUtils::getFirst(tickerNameToBarsMap).value().second.size();
					std::vector<MoneyType> gain_history(vector_size, MoneyType{});
					
					for (const auto& [ticker_name, bars] : tickerNameToBarsMap) {
						
						StrategyType strategy(stoploss_percentage_to_buy, stoploss_percentage_to_sell);
						
						const Ticker ticker;
						const OrderLogger logger = ticker.RunTest(strategy, balance, bars);
						
						const std::vector<MoneyType>& bar_end_net_worths = logger.bar_end_net_worths;
						
						for (size_t i = 0; i < gain_history.size(); i++) {
							gain_history[i] += bar_end_net_worths[i];
						}
						
						const MoneyType final_balance = CollectionUtils::getLast(bar_end_net_worths).value();
						sum_of_total_balances += final_balance;
						
					}
					
					const double gain = sum_of_total_balances / (balance * tickerNameToBarsMap.size());
					report << gain << TAB;
					
					std::for_each(gain_history.begin(), gain_history.end(), [&tickerNameToBarsMap, balance](MoneyType& net_worth){
						net_worth /= balance * tickerNameToBarsMap.size();
					});
					
					TestResultSameParameterMultiStock testResult;
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
