//
//  utils.h
//  BorsaAnaliz
//
//  Created by Samet Pilav on 24.04.2021.
//

#ifndef utils_h
#define utils_h

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>
#include <optional>
#include <iterator>
#include <chrono>
#include <array>
#include <memory>
#include <stdexcept>
#include <sstream>

#include <ctime>

namespace ba {

	struct CollectionUtils
	{
		template<typename Collection>
		static auto getFirst(const Collection& collection) {
			return collection.size() != 0 ? std::optional{ *collection.begin() } : std::nullopt;
		}

		template<typename Collection>
		static auto getLast(const Collection& collection) {
			return collection.size() != 0 ? std::optional{ *collection.rbegin() } : std::nullopt;
		}
		
		template<typename Collection, typename Predicate>
		static auto filter(const Collection& collection, Predicate predicate) {
			
			Collection filtered;
			
			std::copy_if(collection.begin(), collection.end(), std::back_inserter(filtered), predicate);
			
			return filtered;
		}
	};

	struct StringUtils {
		
		static std::vector<std::string> split(const std::string& data, char delimiter) {
			std::vector<std::string> strings;
			
			auto iter_begin = data.begin();
			auto iter_end = data.begin();
			
			while (true) {
				iter_end = std::find(iter_begin, data.end(), delimiter);
				if (iter_end == data.end()) {
					break;
				}
				strings.push_back( std::string(iter_begin, iter_end) );
				iter_begin = iter_end + 1;
			}
			
			return strings;
		}
	};

	struct RangeUtils {
		
		template<typename T>
		static std::vector<T> range(T begin, T end, T step = 1) {
			
			std::vector<T> data;
			data.reserve((end - begin)/step+1);
			
			if (begin < end && 0 < step) {
				for (T value = begin; value < end; value += step) {
					data.push_back(value);
				}
			}
			else if (end < begin && step < 0) {
				for (T value = begin; value > end; value -= step) {
					data.push_back(value);
				}
			}
			return data;
		}
	};

	struct TimeUtils {
		
		static time_t epoch() {
			return time(NULL);
		}
		
		static std::string dateStringFromEpoch(const time_t epoch) {
			std::ostringstream ss;
			const struct tm* date_time = gmtime(&epoch);
			ss << (date_time->tm_year + 1900) << "-" << (date_time->tm_mon + 1) << "-" << date_time->tm_mday;
			return ss.str();
		}
		
		static time_t epochFromDateString(const std::string& dateString) {
			assert(dateString.size() == 10);
			struct tm date_time = {0};
			date_time.tm_year = std::stoi(std::string{std::begin(dateString)  , std::begin(dateString)+4}) - 1900;
			date_time.tm_mon  = std::stoi(std::string{std::begin(dateString)+5, std::begin(dateString)+7}) - 1;
			date_time.tm_mday = std::stoi(std::string{std::begin(dateString)+8, std::end(dateString)    });
			return mktime(&date_time);
		}
		
		static std::string epochStringFromDateString(const std::string& dateString) {
			const time_t epoch = epochFromDateString(dateString);
			return std::to_string(epoch);
		}
	};

	struct DataUtils {
		
	private:
		
		static std::string buildApiUrl(const std::string& ticker, const std::string& period1, const std::string& period2, const std::string& interval) {
			return "https://query1.finance.yahoo.com/v7/finance/download/" + ticker + "?period1=" + period1 + "&period2=" + period2 + "&interval=" + interval + "&events=history&includeAdjustedClose=true";
		}
		
		static std::string downloadData(const std::string& url) {
			
			std::string cmd = "curl -s \"" + url + "\"";
			std::array<char, 128> buffer = {0};
			std::string result;
			std::unique_ptr<FILE, decltype(&pclose)> pipe = {popen(cmd.c_str(), "r"), pclose};
			if (!pipe) {
				throw std::runtime_error("popen() failed!");
			}
			while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
				result += buffer.data();
			}
			return result;
		}
		
		static std::vector<ba::Bar> dataToBars(const std::string& data) {
			
			const auto lines = StringUtils::split(data, '\n');
			
			if (lines.size() < 1 || lines[0] != "Date,Open,High,Low,Close,Adj Close,Volume") {
				return {};
			}
			
			std::vector<ba::Bar> bars;
			
			std::for_each(lines.begin()+1, lines.end(), [&bars](const std::string& line){
				
				const auto cells = StringUtils::split(line, ',');
				
				if (cells.size() != 6)
					return;
				
				bars.emplace_back(ba::Bar {
					.date  = cells[0],
					.open  = std::stof(cells[1]),
					.high  = std::stof(cells[2]),
					.low   = std::stof(cells[3]),
					.close = std::stof(cells[4])
				});
				
			});
			
			return bars;
		}
		
	public:
		
		static std::vector<Bar> getBars(const std::string& ticker, const std::string& period1,
										const std::string& period2, const std::string& interval) {
			auto url = DataUtils::buildApiUrl(ticker, period1, period2, interval);
			auto data = DataUtils::downloadData(url);
			auto bars = DataUtils::dataToBars(data);
			return bars;
		}
	};

	struct BarUtils
	{
		
	public:
		
		static inline double buyBeginSellEndGain(const std::vector<Bar>& bars) {
			return CollectionUtils::getLast(bars).value().close / CollectionUtils::getFirst(bars).value().open;
		}

		static inline MoneyType calculateStep(const MoneyType price) {
			return price > 100 ? 0.10f : price > 50 ? 0.05f : price > 20 ? 0.02f : 0.01f;
		}

		static inline MoneyType correctPrice(const MoneyType price) {

			const MoneyDiffType step = calculateStep(price);
			const int price_i = int(price * 100);
			const int step_i = int(step * 100);
			const int fazlasi_i = price_i % step_i;
			const int final_i = price_i - fazlasi_i;

			return final_i / 100.0f;
		}
		
		static inline MoneyType keepInRange(MoneyType low, MoneyType price, MoneyType high) {
			return std::min(std::max(low, price), high);
		}
		
		static std::vector<float> normalize(const std::vector<MoneyType>& moneyVec) {
			std::vector<float> normalizedVec;
			normalizedVec.reserve(moneyVec.size());
			const auto firstVal = CollectionUtils::getFirst(moneyVec).value_or(1);
			for (const auto money : moneyVec) {
				normalizedVec.push_back(money / firstVal);
			}
			return normalizedVec;
		}
		
	};

	struct ReportUtils {
		
		static void report(std::ostream& os, const std::vector<ba::TestResult>& results, const size_t best_n_results) {
			
			os << std::setw(11) << "Balance " << std::setw(11) << "Upper % " << std::setw(11) << "Lower % " << std::setw(11) << "Orders " << std::setw(11) << "Karli %\n";

//			const OrderLogger& logger = CollectionUtils::getFirst(results).value().orderLogger;
//			const std::vector<OrderLog>& orderLogs = logger.orderLogs;
//			for (const auto& orderLog : orderLogs) {
//				os << "\t\t" << borsa::to_string(orderLog.orderType) << "\tat " << orderLog.barNo << "\t" << orderLog.netWorth << "\n";
//			}
//			os << "\n";
			
//			for (const auto& bar_end_net_worth : logger.bar_end_net_worths) {
//				os << "\t\t" << bar_end_net_worth << "\n";
//			}
//			os << "\n";
			
			std::for_each_n(results.begin(), std::min(results.size(), best_n_results), [&os](const ba::TestResult& testResult){
				
				const auto& orderLogs = testResult.orderLogger.orderLogs;
				
				size_t order_count = std::count_if(orderLogs.begin(), orderLogs.end(), [](const ba::OrderLog& log){
					return log.orderType != ba::OrderType::None;
				});
				
				int karli_islem = 0;
				for (size_t i = 0; i < orderLogs.size(); i++) {
					if (orderLogs[i].orderType == OrderType::ClosePosition) {
						karli_islem += (orderLogs[i-1].netWorth < orderLogs[i].netWorth);
						os << "\t\t" << orderLogs[i-1].netWorth << " / " << orderLogs[i].netWorth << "\n";
					}
				}
				
				os	<< std::setw(10) << testResult.netWorth << " "
					<< std::setw(10) << testResult.var1 << " "
					<< std::setw(10) << testResult.var2 << " "
					<< std::setw(10) << order_count << " "
					<< std::setw(10) << (100 * karli_islem / order_count) << "\n";
			});
		}
		
		static void report(std::ostream& os, const std::vector<ba::TestResultSameParameterMultiStock>& results) {
			
			os.imbue(std::locale("de_DE"));
			
			ba::MoneyType max_gain = 0;
			
			for (const auto& result : results) {
				
//				std::cout << "Buy: " << result.stoploss_percentage_to_buy << " - Sell: " << result.stoploss_percentage_to_sell << "\n";
				
//				std::stringstream file_name_stream;
//				file_name_stream << result.stoploss_percentage_to_buy << "-" << result.stoploss_percentage_to_sell << ".txt";
				
				
				os << "\n";
				os << result.stoploss_percentage_to_buy << "\t" << result.stoploss_percentage_to_sell << "\t\t";
				for (ba::MoneyType net_worth : result.gain_history) {
					os << net_worth << "\t";
				}
				os << "\n";
				
				ba::MoneyType gain = ba::CollectionUtils::getLast(result.gain_history).value_or(0);
				max_gain = std::max(gain, max_gain);
			}
			
			os << "\nMax gain: " << max_gain << "\n";
		}
		
	};

}


#endif /* utils_h */
