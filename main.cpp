//
//  main.cpp
//  BorsaAnaliz
//
//  Created by Samet Pilav on 24.01.2023.
//

#include <vector>
#include <string>
#include <map>

#include "borsa/borsa.h"

#include "TrailingStoplossStrategy.h"
#include "UnitStrategy.h"
#include "LessLossStrategy.h"
#include "OttStrategy.h"
#include "OcoStrategy.h"


auto getTickerNames() -> std::vector<std::string> {
	return {"KONTR.IS"};
}

auto getTickerToBarsMap(const std::vector<std::string>& ticker_names, const std::string& start_date, const std::string& end_date)  {
	
	using namespace ba;
	using namespace std::string_literals;
	
	std::map<std::string, std::vector<Bar>> ticker_to_bars_map;
	
	const auto period1 = TimeUtils::epochStringFromDateString(start_date);
	const auto period2 = TimeUtils::epochStringFromDateString(end_date);
	const auto interval = "1d"s;
	
	for (const auto& ticker_name : ticker_names) {
		
		auto bars = DataUtils::getBars(ticker_name, period1, period2, interval);
		ticker_to_bars_map.insert(std::make_pair(ticker_name, std::move(bars)));
	}
	
	return ticker_to_bars_map;
}

int main(int argc, const char * argv[]) {
	
	using namespace ba;
	using namespace ba::tester;
	using namespace std::string_literals;
	
	const auto ticker_names = getTickerNames();
	auto ticker_to_bars_map = getTickerToBarsMap(ticker_names, "2020-01-01", "2021-01-01");
	
	const auto output_file_name = "out.csv"s;
	
	const auto results = RunTestWithSameParamsOnEveryStock< TrailingStoplossStrategy >( ticker_to_bars_map, output_file_name );
	
	return 0;
}
