//
//  main.cpp
//  BorsaAnaliz
//
//  Created by Samet Pilav on 24.01.2023.
//

#include <vector>

#include "borsa/borsa.h"

#include "TrailingStoplossStrategy.h"
#include "UnitStrategy.h"
#include "LessLossStrategy.h"
#include "OttStrategy.h"
#include "OcoStrategy.h"

// run test for one stock and one strategy parameters
void example_1() {
	
	using namespace ba;
	
	// get bars & run test
	const auto bars = DataUtils::getBars("ARCLK.IS", "2020-01-01", "2023-01-01");
	const auto summary = Tester::RunTest(TrailingStoplossStrategy{3, 7}, bars, MoneyType{10'000}, CommissionRateType{0.15});
	
	// obtain results
	const auto orders_made = summary.totalOrders;
	const auto final_balance = summary.finalBalance;
	
	// print results
	std::cout << orders_made << " orders made and final balance is " << final_balance << "\n";
}

// run test for one stock and many strategy parameters to find the best parameters
void example_2() {
	
	using namespace ba;
	
	// define ranges for strategy parameters to optimize
	const auto permutations = RangeUtils::permutations(std::vector{
		RangeUtils::range<ParamType>(1, 10),
		RangeUtils::range<ParamType>(1, 10, .1)});
	
	// get bars & run test using many strategy parameters
	const auto bars = DataUtils::getBars("ARCLK.IS", "2020-01-01", "2023-01-01");
	const auto summaries = Tester::RunTestUsingParamPermutations<TrailingStoplossStrategy>(
		permutations,
		bars,
		MoneyType{10'000},
		CommissionRateType{0.15});
	
	// take first 4 summaries
	for (auto summary = summaries.begin(); summary != summaries.begin() + 4; ++summary) {
		// obtain results
		const auto orders_made = summary->totalOrders;
		const auto final_balance = summary->finalBalance;
		auto&& params = summary->params;
		// print results
		for (auto param : params) {
			std::cout << param << " ";
		}
		std::cout << "parameters caused " << orders_made << " orders and final balance is " << final_balance << "\n";
	}
}

// run test for many stock and many strategy parameters to find the best generalized parameters
void example_3() {
	
	using namespace ba;
	
	// get bars
	auto ticker_name_to_bars_map = DataUtils::getBars({"ARCLK.IS", "YKBNK.IS", "FROTO.IS"}, "2020-01-01", "2023-01-01");
	
	// run test for all & write into a file
	Tester::RunTestOnManyStocksForGeneralOptimization<TrailingStoplossStrategy>(
		ticker_name_to_bars_map,
		RangeUtils::range<ParamType>(.1, 10, .1),
		RangeUtils::range<ParamType>(.1, 10, .1),
		MoneyType{10'000},
		CommissionRateType{0.15},
		"result.csv");
	
}

int main(int argc, const char * argv[]) {
	
	example_1();
	
	//example_2();
	
	//example_3();
	
	return 0;
}
