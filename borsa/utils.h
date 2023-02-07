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
#include <ctime>
#include <cstring>

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
#include <numeric>
#include <map>


namespace ba {

	struct CollectionUtils
	{
		template<typename Collection>
		static auto getFirst(const Collection& collection) noexcept {
			return collection.size() != 0 ? std::optional{ *collection.begin() } : std::nullopt;
		}

		template<typename Collection>
		static auto getLast(const Collection& collection) noexcept {
			return collection.size() != 0 ? std::optional{ *collection.rbegin() } : std::nullopt;
		}
		
		template<typename Collection, typename Predicate>
		static auto filter(const Collection& collection, Predicate predicate) noexcept {
			
			Collection filtered;
			
			std::copy_if(collection.begin(), collection.end(), std::back_inserter(filtered), predicate);
			
			return filtered;
		}
	};

	struct StringUtils {
		
		static std::vector<std::string> split(const std::string& data, char delimiter) noexcept {
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
		static std::vector<T> range(T begin, T end, T step = 1) noexcept {
			
			std::vector<T> data;
			data.reserve((end - begin)/step+1);
			
			if (begin < end && 0 < step) {
				end += 0.001;
				for (T value = begin; value < end; value += step) {
					data.push_back(value);
				}
			}
			else if (end < begin && step < 0) {
				end -= 0.001;
				for (T value = begin; value > end; value += step) {
					data.push_back(value);
				}
			}
			return data;
		}
		
		template<typename T>
		static
		std::vector<std::vector<T>> permutations(const std::vector<std::vector<T>>& rangeVec) noexcept {
			
			if (rangeVec.empty()) {
				return {};
			}
			
			const auto fn_size_accumulator = [](size_t s, auto& v1) -> size_t { return s * v1.size(); };
			const size_t result_size = std::accumulate(rangeVec.begin(), rangeVec.end(), 1, fn_size_accumulator);
			
			std::vector<T> temp_stack;
			temp_stack.reserve(rangeVec.size());
			
			std::vector<std::vector<T>> result;
			result.reserve(result_size);
			RangeUtils::calculate_permutation_recursively(0, rangeVec, temp_stack, result);
			return result;
		}
		
	private:
		template<typename T>
		static
		void calculate_permutation_recursively(const size_t depth, const std::vector<std::vector<T>>& ranges,
			std::vector<T>& tempStack, std::vector<std::vector<T>>& result) noexcept {
			
			if (depth == ranges.size()) {
				result.push_back(tempStack);
				return;
			}
			auto& v = ranges[depth];
			for (auto n : v) {
				tempStack.push_back(n);
				calculate_permutation_recursively(depth+1, ranges, tempStack, result);
				tempStack.pop_back();
			}
		}
	};

	struct TimeUtils {
		
		static time_t epoch() {
			return time(NULL);
		}
		
		static std::string dateStringFromEpoch(const time_t epoch) noexcept {
			std::ostringstream ss;
			const struct tm* date_time = gmtime(&epoch);
			ss << (date_time->tm_year + 1900) << "-" << (date_time->tm_mon + 1) << "-" << date_time->tm_mday;
			return ss.str();
		}
		
		static time_t epochFromDateString(const std::string& dateString) noexcept {
			assert(dateString.size() == 10);
			struct tm date_time = {0};
			date_time.tm_year = std::stoi(std::string{std::begin(dateString)  , std::begin(dateString)+4}) - 1900;
			date_time.tm_mon  = std::stoi(std::string{std::begin(dateString)+5, std::begin(dateString)+7}) - 1;
			date_time.tm_mday = std::stoi(std::string{std::begin(dateString)+8, std::end(dateString)    });
			return mktime(&date_time);
		}
		
		static std::string epochStringFromDateString(const std::string& dateString) noexcept {
			const time_t epoch = epochFromDateString(dateString);
			return std::to_string(epoch);
		}
		
		static std::string dateTimeNow() noexcept {
			const time_t now = epoch();
			const struct tm* date_time = gmtime(&now);
			std::ostringstream ss;
			ss << std::setfill('0')
			<< (date_time->tm_year + 1900) << "-"
			<< std::setw(2) << (date_time->tm_mon + 1) << "-"
			<< std::setw(2) << date_time->tm_mday << " "
			<< std::setw(2) << date_time->tm_hour << ":"
			<< std::setw(2) << date_time->tm_min << ":"
			<< std::setw(2) << date_time->tm_sec;
			return ss.str();
		}
		
		static std::string dateTimeNowFileFormat() noexcept {
			const time_t now = epoch();
			const struct tm* date_time = gmtime(&now);
			std::ostringstream ss;
			ss << std::setfill('0')
			<< (date_time->tm_year + 1900)
			<< std::setw(2) << (date_time->tm_mon + 1)
			<< std::setw(2) << date_time->tm_mday << "_"
			<< std::setw(2) << date_time->tm_hour
			<< std::setw(2) << date_time->tm_min
			<< std::setw(2) << date_time->tm_sec;
			return ss.str();
		}
	};

	struct DataUtils {
		
	private:
		
		static std::string buildApiUrl(const std::string& ticker, const std::string& period1, const std::string& period2) noexcept {
			return "https://query1.finance.yahoo.com/v7/finance/download/" + ticker + "?period1=" + period1 + "&period2=" + period2 + "&interval=1d&events=history&includeAdjustedClose=true";
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
		
		static std::vector<Bar> dataToBars(const std::string& data) {
			
			const auto lines = StringUtils::split(data, '\n');
			
			if (lines.size() < 1 || lines[0] != "Date,Open,High,Low,Close,Adj Close,Volume") {
				return {};
			}
			
			std::vector<Bar> bars;
			
			std::for_each(lines.begin()+1, lines.end(), [&bars](const std::string& line){
				
				const auto cells = StringUtils::split(line, ',');
				
				if (cells.size() != 6)
					return;
				
				bars.emplace_back(Bar {
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
		
		static auto getBars(const std::string& ticker_name,
							const std::string& first_date,
							const std::string& last_date)
		{
			const auto period1 = TimeUtils::epochStringFromDateString(first_date);
			const auto period2 = TimeUtils::epochStringFromDateString(last_date);
			const auto url = DataUtils::buildApiUrl(ticker_name, period1, period2);
			const auto data = DataUtils::downloadData(url);
			auto bars = DataUtils::dataToBars(data);
			return bars;
		}
		
		static auto getBars(const std::vector<std::string>& ticker_names,
							const std::string& first_date,
							const std::string& last_date)
		{
			std::map<std::string, std::vector<Bar>> ticker_name_to_bars_map;
			for (auto&& ticker_name : ticker_names) {
				auto bars = getBars(ticker_name, first_date, last_date);
				ticker_name_to_bars_map.insert(std::make_pair(ticker_name, std::move(bars)));
			}
			return ticker_name_to_bars_map;
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
			const int remaining_i = price_i % step_i;
			const int final_i = price_i - remaining_i;

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

}


#endif /* utils_h */
